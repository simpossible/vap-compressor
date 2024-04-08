import { en, th } from "element-plus/es/locale";
import { FileNode } from "./file_node";
import { CompressState } from "../file_server/compress_state"
import { error } from "console";
import { Nullable } from "element-plus/es/utils";


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

interface CompressTaskStateInterface {
  taskStateChanged: (task: CompressTask) => void;
}

class CompressTask {
  node: FileNode;
  compressInfo: any = {};
  resolution: string = ""; // 分辨率
  duration: string = ""; // 时长
  compressParams: any = {}; // 压缩参数
  taskState: CompressTaskState = CompressTaskState.done; // 任务状态
  error: string = ""; // 错误信息  

  compressedFileInfo: any | null = null; // 压缩后的文件
  compressStateStr: string = ""; // 压缩状态
  orgFileSizeStr: string = ""; // 原始文件大小
  orgBitRateStr: string = ""; // 原始码率
  compressedFileSizeStr: string = ""; // 压缩后文件大小
  compressedBitRateStr: string = ""; // 压缩后码率
  displayPath: string = ""; // 显示路径

  delegate: CompressTaskStateInterface | null = null;

  constructor(node: FileNode) {
    this.node = node;
    this.node.addCompresseDelegate(this);
  }
  start(compressParams: any) {
    // 先加载压缩的信息
    this.compressParams = compressParams;
    this.node.loadCompressInfo();
    this.refreshInfos()
  }

  refreshInfos() {
    var fileSizeBytes = this.node.fileInfo.size;
    this.resolution = this.node.fileInfo.video_info.width + "x" + this.node.fileInfo.video_info.height;
    this.duration = this.formatTime(this.node.fileInfo.video_info.duration_ts)
    this.orgFileSizeStr = this.formatBytes(fileSizeBytes)
    this.orgBitRateStr = this.node.fileInfo.video_info.bit_rate
    if (this.compressedFileInfo != null) {
      var compresscfileSizeBytes = this.compressedFileInfo.size;
      this.compressedFileSizeStr = this.formatBytes(compresscfileSizeBytes)
      this.compressedBitRateStr = this.compressedFileInfo.video_info.bit_rate
    }
  }
  formatBytes(bytes) {
    if (bytes < 1024) {
      return bytes + " Bytes";
    } else if (bytes < 1048576) {
      return (bytes / 1024).toFixed(2) + " KB";
    } else {
      return (bytes / 1048576).toFixed(2) + " MB";
    }
  }
  formatTime(seconds: number) {
    let hours = Math.floor(seconds / 3600);
    let minutes = Math.floor((seconds % 3600) / 60);
    let secs = parseFloat((seconds % 60).toFixed(1));

    let timeStr = "";

    if (hours > 0) {
      timeStr += hours + "小时";
    }
    if (minutes > 0) {
      timeStr += minutes + "分钟";
    }
    if (secs > 0) {
      timeStr += secs + "秒";
    }

    return timeStr;
  }

  onCompressInfoLoaded(node: FileNode) {
    if (node.src == this.node.src) {
      this.compressInfo = node.compressInfo
      if (this.compressInfo.outputFileInfo != undefined) {
        this.compressedFileInfo = this.compressInfo.outputFileInfo
      }
      var func = statusDealerMap.get(this.compressInfo.compressState);
      if (func) {
        func(this);
      }
      this.delegate?.taskStateChanged(this);
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
    this.compressedNode = new FileNode(this.compressInfo.outputPath);
    this.compressedNode.isOutputNode = true
    this.compressedNode.delegate = this
    this.compressedNode.initialData()
  }
}

function statusDealerForAcceptting(task: CompressTask) {

}

function statusDealerForQuitting(task: CompressTask) {

}

export { CompressTask }