import { FileNode, FileNodeType } from "../../sdk/file_node";
import * as path from 'path-browserify';
import { shared_center } from '../../sdk/vap_center';

interface CellNodeVmUIEvent {
    toUpdateNodeUI: (vm: any) => void;
}
class CellNodeVm {
    node: FileNode;
    label: string = "";
    children: CellNodeVm[] = [];
    childrenMap: Map<string, CellNodeVm> = new Map();
    uuid: string = "";
    errorTip: string = "";  //错误提示
    delegate: CellNodeVmUIEvent | null = null;
    constructor(node: FileNode) {
        this.node = node;
        this.uuid = node.src;
        this.label = path.basename(node.src);
        this.errorTip = "";
        for (const child of node.subNodes) {
            const vm = new CellNodeVm(child)
            this.children.push(vm);
            this.childrenMap.set(child.src, vm);
        }
        node.delegate = this
        node.initialData();
    }

    setDelegate(delegate: CellNodeVmUIEvent) {
        this.delegate = delegate;
        for (const child of this.children) {
            child.setDelegate(delegate);
        }
    }

    isVap() {
        return this.node.fileType == FileNodeType.vap;
    }

    onNodeInfoLoaded(node: FileNode) {
        console.log("onNodeInfoLoaded a ");
        if (this.node.fileType !== FileNodeType.vap && this.node.fileType !== FileNodeType.dir) {
            this.errorTip = "不支持的文件类型";
            if (this.delegate != null) {
                this.delegate.toUpdateNodeUI(this);
            }
            return;
        }
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