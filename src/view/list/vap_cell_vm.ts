import FileNode from "../../sdk/file_node";
class CellNodeVm {
    node: FileNode;
    label: string = "";
    children: CellNodeVm[] = [];
    constructor(node: FileNode) {
        this.node = node;
    }
}