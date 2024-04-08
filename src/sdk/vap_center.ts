import { FileNode } from './file_node';
import { isMP4File } from './file_util';
import { setNodeCache } from './node_cache';
import { vapUrlForKey } from './url_config';

interface VapCenterInterface {
  onNodeAdded: (node: FileNode) => void;
}

class VapCenter {
  nodes: FileNode[];
  nodeMap: Map<string, FileNode> = new Map();
  delegate: VapCenterInterface | null = null;
  constructor() {
    this.nodes = [];
    this.nodeMap = new Map();
    setNodeCache(this);
  }


  createFileNodeFromFiles(files) {
    console.log("createFileNodeFromFiles");
    for (let file of files) {
      var node = this.getNodeByPath(file.path);
      if (node !== null) {
        this.nodes.unshift(node);
        if (this.delegate !== null) {
          this.delegate.onNodeAdded(node);
        }
      }
    }
  }
  getNodeByPath(path: string): FileNode | null {
    var node;
    if (this.nodeMap.has(path)) {
      node = this.nodeMap.get(path);
    } else {
      node = new FileNode(path);
      this.nodeMap.set(path, node);
    }
    return node;
  }

  cacheNode(node: any, path: string): void {
    this.nodeMap.set(path, node);
  }
}

let shared_center = new VapCenter();
export { VapCenter, VapCenterInterface, shared_center };
