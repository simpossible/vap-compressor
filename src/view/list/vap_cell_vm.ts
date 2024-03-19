import FileNode from "../../sdk/file_node";
class NodeVm {
    node: FileNode;
    label: string = "";
    children: NodeVm[] = [];
    constructor(node: FileNode) {
        this.node = node;
    }
}