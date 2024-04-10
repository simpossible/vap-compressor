import axios from 'axios';
import { FileNode } from './file_node';
import { isMP4File } from './file_util';
import { setNodeCache } from './node_cache';
import { UrlPathVapClearFiles, vapUrlForKey } from './url_config';

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

  clearCompressInfoForFiles(pathList) {
    // 删除这些文件的压缩信息 compressinfo
    var params = { files: pathList };
    var url = vapUrlForKey(UrlPathVapClearFiles)    
    // 使用axios发送post请求content-type为application/json
    return axios.post(url, params, {
      headers: {
        'Content-Type': 'application/json'
      }
    })    
  }
}

let shared_center = new VapCenter();
export { VapCenter, VapCenterInterface, shared_center };
