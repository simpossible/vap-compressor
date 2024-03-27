import { fi } from "element-plus/es/locale";
import axios from 'axios';

interface FileNodeInterface {
    onNodeInfoLoaded: (node: FileNode) => void;
}

interface FileNodeCompressInterFace {
    onNodeCompressInfoUpdated: (node: FileNode) => void;
}

enum FileNodeType {
    unknow,
    dir,
    vap
}

class FileNode {
    src: string;    
    subNodes: FileNode[] = [];
    subNodesMap: Map<string, FileNode> = new Map();
    fileInfo: any = {};
    delegate: any = null;
    isLoading: boolean = false;
    compressInfo: any = {};
    compressDelegates: Array<any> = [];
    isCompressingLoadding: boolean = false;
    isCompressing: boolean = false;
    fileType: FileNodeType = FileNodeType.unknow
    constructor(src: string) {
        this.src = src;
    }

    addCompresseDelegate(delegate: any){
        if (this.compressDelegates.indexOf(delegate) >= 0){
            return
        }
        this.compressDelegates.push(delegate);
    }
    deleteCompresseDelegate(delegate: any){
        var index = this.compressDelegates.indexOf(delegate);
        if (index >= 0){
            this.compressDelegates.splice(index, 1);
        }
    }



    initialData() {
        if (this.isLoading) {
            return
        }
        this.isLoading = true;
        axios.get('http://127.0.0.1:3000/file?path=' + this.src)
            .then(response => {
                this.isLoading = false;
                var responseJson = response.data;
                var subFiles = responseJson["sub_files"]
                var tempSubMap = new Map();
                var isDir = responseJson["is_dir"];
                var isVap = responseJson["is_vap"];
                if (isDir){
                    this.fileType = FileNodeType.dir;
                }else {
                    if (isVap){
                        this.fileType = FileNodeType.vap;
                        this.fileInfo = responseJson["file_info"];
                    }
                }
                var tempArray: FileNode[] = [];
                for (let subFile of subFiles) {
                    var newNode : FileNode | undefined = undefined;
                    if (this.subNodesMap.has(subFile)) {
                        newNode = this.subNodesMap.get(subFile);
                    }else {
                        newNode = new FileNode(subFile);
                    }
                    if (newNode !== undefined) {
                        tempArray.push(newNode);
                        tempSubMap.set(subFile, newNode);
                    }                                                    
                }
                this.subNodes = tempArray;
                this.subNodesMap = tempSubMap;
                if (this.delegate !== null) {
                    this.delegate.onNodeInfoLoaded(this);
                }
            })
            .catch(error => {
                this.isLoading = false;
                console.log(error);
            });
    }
    loadCompressInfo(){
        // get current compress info
        if  (this.isCompressingLoadding){
            return
        }
        this.isCompressingLoadding = true
        axios.get('http://127.0.0.1:3000/compress-info?path=' + this.src).then(response => {
            this.isCompressingLoadding = false
            this.compressInfo = response.data;
            for(let delegate of this.compressDelegates){
                delegate.onNodeCompressInfoUpdated(this);
            }
        }).catch(error => {
            this.isCompressingLoadding = false
        })
    }

    startCompress(){
        if (this.isCompressing){
            return
        }
        if (this.fileType != FileNodeType.vap){
            return
        }
        if (this.compressInfo.state == 1) {
            return
        }
        this.isCompressing = true
        axios.get('http://127.0.0.1:3000/start-compress?path=' + this.src).then(response => {
            this.isCompressing = false
            this.loadCompressInfo();
        }).catch(error => {
            this.isCompressing = false
        })

    }
}



export  {FileNode, FileNodeType};
