import { en, th } from "element-plus/es/locale";
import { FileNode } from "./file_node";

enum CompressTaskStatus {
  none = 0,
  compressing = 1,
  merging = 2,
  finished = 3,
}

class CompressTask {
    node: FileNode;
    compressedNode: FileNode | null = null;
    compressInfo: any = {};

    orgFileSize: number = 0; // 原始文件大小
    orgBitRate: number = 0; // 原始码率
    resolution: string = ""; // 分辨率
    compressedFileSize: number = 0; // 压缩后文件大小
    compressedBitRate: number = 0; // 压缩后码率
    compressStateStr: string = ""; // 压缩状态
    taskStatus: number = CompressTaskStatus.none; // task状态
    compressParams: any = {}; // 压缩参数
    constructor(node: FileNode) {
        this.node = node;
        this.node.addCompresseDelegate(this);
    }
    start(compressParams: any) {
      // 先加载压缩的信息
      this.compressParams = compressParams;
      this.node.loadCompressInfo();
    }

    onCompressInfoLoaded(node: FileNode) {
        if (node.src == this.node.src) {
          this.compressInfo = node.compressInfo
          if (this.compressInfo.state == 0) {
              this.node.startCompress(this.compressParams);
          }
        }
    }
}