import { fi } from "element-plus/es/locale";

const fs = require('fs');
const path = require('path');

class FileNode {
    src: string;
    constructor(src: string) {
        this.src = src;
    }

    initial() {
        // get the file info
        if (!fs.lstatSync(this.src).isFile()) {
            return;
        }
    }
}


export default FileNode;


