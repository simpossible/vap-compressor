import FileNode from "../../sdk/file_node";
import * as path from 'path';
import {shared_center} from '../../sdk/vap_center';
class CellNodeVm {
    node: FileNode;
    label: string = "";
    children: CellNodeVm[] = [];   
    uuid: string = "" 
    constructor(node: FileNode) {
        this.node = node;
        this.uuid = node.src;
        this.label = path.basename(node.src);
        for (let child of node.subNodes) {
            this.children.push(new CellNodeVm(child));
        }
        node.delegate = this
        node.initialData();
    }

    onNodeInfoLoaded(node: FileNode) {   
        console.log("onNodeInfoLoaded");
        for (let child of node.subNodes) {
            this.children.push(new CellNodeVm(child));
        }
        shared_center.delegate.$refs.vap_list_tree.updateKeyChildren(node.src, this.children)
    }

}

export default CellNodeVm;