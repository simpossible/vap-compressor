import FileNode from './file_node';

class VapCenter {
  nodes: FileNode[];
  constructor() {
    this.nodes = [];
  }

  addNodeForPath(src: string): number {
    let [code, node] = this.createFileNode(src);
    if (code == 0) {
      // add node to first
      if (node !== null) {
        this.nodes.unshift(node);
      }else {
        return -999;
      }
      return code;
    }
    return code;

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

 
}