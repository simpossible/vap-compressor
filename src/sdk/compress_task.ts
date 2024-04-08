import { en, th } from "element-plus/es/locale";
import { FileNode } from "./file_node";
import { CompressState } from "../file_server/compress_state"
import { error } from "console";


var statusDealerMap = new Map<number, Function>([
  [CompressState.None, statusDealerForNone],
  [CompressState.compressing, statusDealerForCompressing],
  [CompressState.done, statusDealerForDone],
  [CompressState.acceptting, statusDealerForAcceptting],
  [CompressState.quitting, statusDealerForQuitting],

]);

enum CompressTaskState {
  none = 0,
  excuting = 1,
  done = 2,

}

class CompressTask {
  node: FileNode;
  compressedNode: FileNode | null = null;
  compressInfo: any = {};
  orgFileSize: number = 0; // 原始文件大小
  orgBitRate: number = 0; // 原始码率
  resolution: string = ""; // 分辨率
  duration: number = 0; // 时长
  compressedFileSize: number = 0; // 压缩后文件大小
  compressedBitRate: number = 0; // 压缩后码率
  compressParams: any = {}; // 压缩参数
  taskState: CompressTaskState = CompressTaskState.done; // 任务状态
  error: string = ""; // 错误信息  

  compressStateStr: string = ""; // 压缩状态
  orgFileSizeStr: string = ""; // 原始文件大小
  orgBitRateStr: string = ""; // 原始码率
  compressedFileSizeStr: string = ""; // 压缩后文件大小
  compressedBitRateStr: string = ""; // 压缩后码率

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
      var func = statusDealerMap.get(this.compressInfo.compressState);
      if (func) {
        func(this);
      }
    }
  }
}

function statusDealerForNone(task: CompressTask) {
  if (task.taskState == CompressTaskState.none) {
    task.taskState = CompressTaskState.excuting;
    task.node.startCompress(task.compressParams);
  } else if (task.taskState == CompressTaskState.excuting) {
    task.taskState = CompressTaskState.done;
  }
}

function statusDealerForCompressing(task: CompressTask) {

}

function statusDealerForDone(task: CompressTask) {
  if (task.taskState == CompressTaskState.excuting) {
    task.node.acceptCompress((code, error) => {
      if (code == -2) {
        task.error = error;
        task.taskState = CompressTaskState.done;
      }
    })

  }
}

function statusDealerForAcceptting(task: CompressTask) {

}

function statusDealerForQuitting(task: CompressTask) {

}

export { CompressTask }