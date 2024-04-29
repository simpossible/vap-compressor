import { FileNode, FileNodeType } from "../../sdk/file_node";
import * as path from 'path-browserify';
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
        for (const child of node.subNodes) {
            const vm = new CellNodeVm(child)
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
        const tempSubMap = new Map();
        const newArray: CellNodeVm[] = [];
        let hasNew = false;
        for (const child of node.subNodes) {
            let vm: CellNodeVm | undefined = undefined;
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
        const hasDelete = this.childrenMap.size > 0;
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