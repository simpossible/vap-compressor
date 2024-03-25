import { fi } from "element-plus/es/locale";
import axios from 'axios';

interface FileNodeDelegate {
    onNodeInfoLoaded: (node: FileNode) => void;
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
    isDir: boolean = false;
    isLoading: boolean = false;
    fileType: FileNodeType = FileNodeType.unknow
    constructor(src: string) {
        this.src = src;
    }

    initialData() {
        if (this.isLoading) {
            return
        }
        this.isLoading = true;
        axios.get('http://127.0.0.1:3000/file?path=' + this.src)
            .then(response => {
                this.isLoading = false;
                console.log("return file is",response.data);
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
}



export default FileNode;


