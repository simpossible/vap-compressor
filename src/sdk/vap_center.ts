import {FileNode} from './file_node';
import { isMP4File } from './file_util';

interface VapCenterInterface {
  onNodeAdded: (node: FileNode) => void;
}

class VapCenter {
  nodes: FileNode[];
  delegate: VapCenterInterface | null = null;
  constructor() {
    this.nodes = [];
  }

  createFileNode(src: string): [number, FileNode | null] {
    let stat = fs.statSync(src)
    if (stat.isDirectory()) {
      return [0, new FileNode(src)];
    }
    //读取文件信息判断是否是 .mp4文件
    if (isMP4File(src)) {
      return [-2, null];
    }
    return [-3, null];
  }

  createFileNodeFromFiles(files) {
    console.log("createFileNodeFromFiles");
    for (let file of files) {
      var node = new FileNode(file.path);
      if (node !== null) {
        this.nodes.unshift(node);
        if (this.delegate !== null) {
          this.delegate.onNodeAdded(node);
        }
      }
    }
  }
}

let shared_center = new VapCenter();
export { VapCenter, VapCenterInterface, shared_center };
