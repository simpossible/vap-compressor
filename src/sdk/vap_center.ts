import FileNode from './file_node';
import fs from 'fs';
import { isMP4File } from './file_util';

const error_map = {
  "-1": "文件不存在",
  "-2": "不支持的文件格式",
  "-3": "未知错误",
}

interface VapCenterInterface  {
  onNodeAdded: (node: FileNode) => void;
}

class VapCenter {
  nodes: FileNode[];
  delegate: VapCenterInterface | null = null;
  constructor() {
    this.nodes = [];
  }

  createFileNode(src: string) : [number, FileNode | null] {
    if (!fs.existsSync(src)) {
        return [-1, null];
    }
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

  createFileNodeFromFiles(files, error_callback) {
    console.log("createFileNodeFromFiles");
    for (let file of files) {
      let [code, node] = this.createFileNode(file);
      if (code == 0) {
        if (node !== null) {
          this.nodes.unshift(node);
          if (this.delegate !== null) {
            this.delegate.onNodeAdded(node);
          }
        }
      }else {
        if (error_callback !== null) {
          error_callback(error_map[code]);
        }
      }
    }
  }
}

let shared_center = new VapCenter();
export { VapCenter, VapCenterInterface , shared_center};
