import { fi } from "element-plus/es/locale";

import * as fs from 'fs';


class FileNode {
    src: string;
    subNodes: FileNode[] = [];
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


