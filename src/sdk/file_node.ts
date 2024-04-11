import axios from 'axios';
import { UrlPathAcceptCompress, UrlPathQuitCompress, UrlPathStartCompress, UrlPathVapInfo, UrlPathVapList, vapUrlForKey } from './url_config';
import { UrlPathFile, UrlPathCompressInfo } from './url_config';
import { currentNodeCache } from './node_cache';
import { CompressState } from '../file_server/compress_state';
import { tr } from 'element-plus/es/locale';

interface FileNodeInterface {
    onNodeInfoLoaded: (node: FileNode) => void;
}

interface FileNodeCompressInterFace {
    onNodeCompressInfoUpdated: (node: FileNode) => void;
    onNodeCompressCleared: (node: FileNode) => void;
}

enum FileNodeType {
    unknow = 0,
    dir = 1,
    vap = 2
}

class FileNode {
    src: string;
    subNodes: FileNode[] = [];
    subNodesMap: Map<string, FileNode> = new Map();
    fileInfo: any = {};
    delegate: any = null;
    isLoading: boolean = false;
    compressInfo: any = {};
    compressDelegates: Array<any | null> = [];
    isCompressingLoadding: boolean = false;
    isCompressing: boolean = false;
    fileType: FileNodeType = FileNodeType.unknow;
    isQuitCompressing: boolean = false;
    isAcceptCompressing: boolean = false;
    isOutputNode: boolean = false; // 是否是输出节点的node    
    delegates: Array<FileNodeInterface> = [];
    initialed: boolean = false

    timer: any = null; // 定时器持续获取压缩信息
    constructor(src: string) {
        this.src = src;
        if (this.src === "/Users/liangjinfeng/Downloads/aaaa/bbb/xxx/video.mp4") {
            console.log("debug -----2")
        }
    }

    addCompresseDelegate(delegate: any) {
        if (this.compressDelegates.indexOf(delegate) >= 0) {
            return
        }
        this.compressDelegates.push(delegate);
    }
    deleteCompresseDelegate(delegate: any) {
        var index = this.compressDelegates.indexOf(delegate);
        if (index >= 0) {
            this.compressDelegates.splice(index, 1);
        }
    }

    addDelegates(delegate: FileNodeInterface) {
        if (this.delegates.indexOf(delegate) >= 0) {
            return
        }
        this.delegates.push(delegate);
    }
    deleteDelegates(delegate: FileNodeInterface) {
        var index = this.delegates.indexOf(delegate);
        if (index >= 0) {
            this.delegates.splice(index, 1);
        }
    }


    initialData() {
        if (this.src === "/Users/liangjinfeng/Downloads/aaaa/bbb/xxx/video.mp4") {
            console.log("debug -----")
        }
        if (this.isLoading) {
            return
        }
        if (this.initialed === true) {
            return
        }
        this.isLoading = true;
        var fileUrl = ""
        if (this.isOutputNode) {
            fileUrl = vapUrlForKey(UrlPathVapInfo, { path: this.src, output: 1 })
        } else {
            var fileUrl = vapUrlForKey(UrlPathFile, { path: this.src })
        }
        axios.get(fileUrl)
            .then(response => {
                this.initialed = true
                this.isLoading = false;
                var responseJson = response.data;
                var subFiles = responseJson["sub_files"]
                var tempSubMap = new Map();
                var isDir = responseJson["is_dir"];
                var isVap = responseJson["is_vap"];
                if (isDir) {
                    this.fileType = FileNodeType.dir;
                } else {
                    if (isVap) {
                        this.fileType = FileNodeType.vap;
                        this.fileInfo = responseJson["file_info"];
                        console.log("file_info_changed 2", this.src)
                    }
                }
                var tempArray: FileNode[] = [];
                for (let subFile of subFiles) {
                    var newNode: FileNode | null = null;
                    newNode = currentNodeCache().getNodeByPath(subFile) as FileNode;
                    if (newNode === null) {
                        newNode = new FileNode(subFile);
                        currentNodeCache()?.cacheNode(newNode, subFile);
                    }
                    if (newNode !== null) {
                        this.subNodesMap.set(subFile, newNode);
                        tempArray.push(newNode);
                    }
                }
                this.subNodes = tempArray;
                this.subNodesMap = tempSubMap;
                if (this.delegate !== null) {
                    this.delegate.onNodeInfoLoaded(this);
                }
                for (let delegate of this.delegates) {
                    delegate.onNodeInfoLoaded(this);
                }
            })
            .catch(error => {
                this.isLoading = false;
                console.log(error);
            });
    }

    loadCompressInfo() {
        // get current compress info
        console.log("loadCompressInfo");
        if (this.isCompressingLoadding) {
            return
        }
        this.isCompressingLoadding = true
        axios.get(vapUrlForKey(UrlPathCompressInfo, { path: this.src })).then(response => {
            this.isCompressingLoadding = false
            this.compressInfo = response.data;
            for (let delegate of this.compressDelegates) {
                delegate.onNodeCompressInfoUpdated(this);
            }
            if (this.compressInfo.state != CompressState.compressing) {
                this.stopTimer()
            }
        }).catch(error => {
            this.isCompressingLoadding = false
        })
    }

    startTimer() {
        if (this.timer != null) {
            return
        }
        this.timer = setInterval(() => {
            this.loadCompressInfo();
        }, 160);
    }
    stopTimer() {
        if (this.timer != null) {
            clearInterval(this.timer);
            this.timer = null;
        }
    }

    startCompress(params) {
        console.log("start compress");
        if (this.isCompressing) {
            return
        }
        if (this.fileType != FileNodeType.vap) {
            return
        }
        if (this.compressInfo.state == 1) {
            return
        }
        params.path = this.src;
        this.isCompressing = true
        axios.get(vapUrlForKey(UrlPathStartCompress, params)).then(response => {
            console.log("start compress 1", response.data);
            this.startTimer()
            this.isCompressing = false
            this.loadCompressInfo();
            this.compressInfo = response.data;
            for (let delegate of this.compressDelegates) {
                delegate.onNodeCompressInfoUpdated(this);
            }
        }).catch(error => {
            console.log("start compress 1", error);
            this.isCompressing = false
        })
    }

    quitCompress(cb: Function) {
        console.log("to quit compress");
        if (this.isQuitCompressing) {
            cb(-1, "操作正在进行中");
            return
        }
        this.isQuitCompressing = true
        axios.get(vapUrlForKey(UrlPathQuitCompress, { path: this.src })).then(response => {
            this.isQuitCompressing = false
            if (cb != null && cb != undefined) {
                cb(0, "");
            }
            this.loadCompressInfo();
        }).catch(error => {
            this.isQuitCompressing = false
            if (cb != null && cb != undefined) {
                cb(-1, "操作失败: " + error);
            }
        })
    }
    acceptCompress(cb: Function) {
        console.log("to accept compress");
        if (this.isAcceptCompressing) {
            cb(-1, "操作正在进行中");
            return
        }
        this.isQuitCompressing = true
        axios.get(vapUrlForKey(UrlPathAcceptCompress, { path: this.src })).then(response => {
            this.isAcceptCompressing = false
            if (cb != null && cb != undefined) {
                cb(0, "");
            }
            this.loadCompressInfo();
            this.initialData();
        }).catch(error => {
            this.isAcceptCompressing = false
            if (cb != null && cb != undefined) {
                cb(-1, "操作失败: " + error);
            }
        })
    }

    getVapList(): Promise<FileNode[]> {
        return new Promise<FileNode[]>((resolve, reject) => {
            var url = vapUrlForKey(UrlPathVapList, { path: this.src });
            axios.get(url)
                .then(response => {
                    console.log("getVapList", response.data);
                    var file_list = response.data["file_list"];
                    var tempArray: FileNode[] = [];
                    for (let fileInfo of file_list) {
                        var file_path = fileInfo["path"];
                        var cachedNode = currentNodeCache().getNodeByPath(file_path);
                        if (cachedNode == null) {
                            cachedNode = new FileNode(file_path);
                            currentNodeCache().cacheNode(cachedNode, file_path);
                            console.log("file_info_changed 1", cachedNode.src)
                            cachedNode.fileInfo = fileInfo;
                        }
                        tempArray.push(cachedNode);
                    }
                    resolve(tempArray);
                })
                .catch(error => {
                    reject(error);
                    console.log("getVapList", error);
                });
        });
    }

    triggerCompressCleared() {
        this.initialed = false
        for (let delegate of this.compressDelegates) {
            delegate?.onNodeCompressCleared(this);
        }
        this.initialData();
    }
}



export { FileNode, FileNodeType };
