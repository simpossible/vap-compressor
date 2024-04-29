import { FileNode, FileNodeType } from "../../sdk/file_node";
import * as path from 'path';
import { shared_center } from '../../sdk/vap_center';
class CellNodeVm {
    node: FileNode;
    label: string = "";
    children: CellNodeVm[] = [];
    childrenMap: Map<string, CellNodeVm> = new Map();
    uuid: string = ""
    constructor(node: FileNode) {
        this.node = node;
        this.uuid = node.src;
        this.label = path.basename(node.src);
        for (let child of node.subNodes) {
            var vm = new CellNodeVm(child)
            this.children.push(vm);
            this.childrenMap.set(child.src, vm);
        }
        node.delegate = this
        node.initialData();
    }

    isVap() {
        return this.node.fileType == FileNodeType.vap;
    }

    onNodeInfoLoaded(node: FileNode) {
        console.log("onNodeInfoLoaded");
        var tempSubMap = new Map();
        var newArray: CellNodeVm[] = [];
        var hasNew = false;
        for (let child of node.subNodes) {
            var vm: CellNodeVm | undefined = undefined;
            if (this.childrenMap.has(child.src)) {
                vm = this.childrenMap.get(child.src);
                this.childrenMap.delete(child.src);
            } else {
                vm = new CellNodeVm(child);
                hasNew = true;
            }
            if (vm != undefined) {
                newArray.push(vm);
                tempSubMap.set(vm.uuid, vm)
            }
        }
        var hasDelete = this.childrenMap.size > 0;
        this.children = newArray;
        this.childrenMap = tempSubMap;
        console.log("update key", this.uuid, " children:", this.children);
        if (hasDelete || hasNew) {
            shared_center.delegate.$refs.vap_list_tree.updateKeyChildren(this.uuid, this.children);
        }

    }

    reloadFiles() {
        this.node.initialData();
    }

}

export default CellNodeVm;