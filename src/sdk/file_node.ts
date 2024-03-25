import { fi } from "element-plus/es/locale";
import axios from 'axios';

interface FileNodeDelegate {
    onNodeInfoLoaded: (node: FileNode) => void;
}

class FileNode {
    src: string;    
    subNodes: FileNode[] = [];
    fileInfo: any = {};
    delegate: any = null;
    constructor(src: string) {
        this.src = src;
    }

    initialData() {
        axios.get('http://127.0.0.1:3000/file?path=' + this.src)
            .then(response => {
                console.log("return file is",response.data);
                var responseJson = response.data;
                var subFiles = responseJson["sub_files"]
                for (let subFile of subFiles) {
                    var subNode = new FileNode(subFile);
                    this.subNodes.push(subNode);
                }
                if (this.delegate !== null) {
                    this.delegate.onNodeInfoLoaded(this);
                }
            })
            .catch(error => {
                console.log(error);
            });
    }
}



export default FileNode;


