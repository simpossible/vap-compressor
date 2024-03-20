import FileNode from "../../sdk/file_node";
import * as path from 'path';
class CellNodeVm {
    node: FileNode;
    label: string = "";
    children: CellNodeVm[] = [];
    constructor(node: FileNode) {
        this.node = node;
        this.label = path.basename(node.src);
        for (let child of node.subNodes) {
            this.children.push(new CellNodeVm(child));
        }
        node.delegate = this
        node.initialData();
    }

    onNodeInfoLoaded(node: FileNode) {
        console.log("onNodeInfoLoaded");
    }

}

export default CellNodeVm;