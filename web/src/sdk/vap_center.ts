import axios from 'axios';
import { FileNode } from './file_node';
import { isMP4File } from './file_util';
import { setNodeCache } from './node_cache';
import { UrlPathVapClearFiles, vapUrlForKey, UrlPathVapDragFiles } from './url_config';

interface VapCenterInterface {
  onNodeAdded: (node: FileNode) => void;
}

class VapCenter {
  nodes: FileNode[];
  nodeMap: Map<string, FileNode> = new Map();
  delegate: VapCenterInterface | null = null;
  dealingNodeSrc: string = '';
  isEverPlayer:boolean = false;
    
  constructor() {
    this.nodes = [];
    this.nodeMap = new Map();
    setNodeCache(this);
  }


  createFileNodeFromFiles(files) {
    console.log("createFileNodeFromFiles1");
    const onDragUrl = vapUrlForKey(UrlPathVapDragFiles);
    axios.get(onDragUrl).then(response => {
     const files = response.data.files
     console.log("createFileNodeFromFiles");
     for (const file of files) {
       const node = this.getNodeByPath(file.path);
       if (node !== null) {
         this.nodes.unshift(node);
         if (this.delegate !== null) {
           this.delegate.onNodeAdded(node);
         }
       }
     }
  }).catch(error => {
      console.log(error);
  })
   
  }
  getNodeByPath(path: string): FileNode | null {
    let node;
    if (this.nodeMap.has(path)) {
      node = this.nodeMap.get(path);
    } else {
      console.log("debugb --");
      if (path.includes("__compress")) {
        console.log("debug b--");
    }
      node = new FileNode(path);
      this.nodeMap.set(path, node);
    }
    return node;
  }

  cacheNode(node: any, path: string): void {
    console.log("debuga --");
    if (path.includes("__compress")) {
      console.log("debug --");
  }
    this.nodeMap.set(path, node);
  }

  clearCompressInfoForFiles(pathList) {
    // 删除这些文件的压缩信息 compressinfo
    const params = { files: pathList };
    const url = vapUrlForKey(UrlPathVapClearFiles)    
    // 使用axios发送post请求content-type为application/json
    return axios.post(url, params, {
      headers: {
        'Content-Type': 'application/json'
      }
    })    
  }
}

const shared_center = new VapCenter();
export { VapCenter, VapCenterInterface, shared_center };
