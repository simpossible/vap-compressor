import { en, th } from "element-plus/es/locale";
import { FileNode } from "./file_node";
import { CompressState } from "../file_server/compress_state"
import { error } from "console";
import { Nullable } from "element-plus/es/utils";
import { randomUUID } from "crypto";
import { compose } from "element-plus/es/components/table/src/util";



export enum CompressTaskState {
  none = 0,
  preparing = 1,
  prepaired = 2,
  excuting = 3,
  done = 4,

}

var ____global_uuid_task = 0;
function getATaskId() {
  return ____global_uuid_task++;
}

interface CompressTaskStateInterface {
  taskStateChanged: (task: CompressTask) => void;
  taskInfoChanged: (task: CompressTask) => void;
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
  refreshKey: string = ""; // 刷新时间
  progressStr: string = ""; // 进度

  delegate: CompressTaskStateInterface | null = null;

  constructor(node: FileNode) {
    this.refreshKey = node.src
    this.node = node;
    this.node.addDelegates(this)
    this.node.addCompresseDelegate(this);
    this.taskState = CompressTaskState.preparing;
    this.node.initialData()
  }

  onNodeInfoLoaded(node) {
    console.log("onNodeInfoLoaded at task")
    this.node.loadCompressInfo();
    this.refreshInfos()
  }
  onNodeCompressInfoUpdated(node) {
    console.log("onNodeCompressInfoUpdated at task")
    this.compressInfo = node.compressInfo
    if (this.taskState == CompressTaskState.preparing) {
      this.taskState = CompressTaskState.prepaired;
    }
    if (this.compressInfo.outputFileInfo != undefined) { // 压缩完成
      this.compressedFileInfo = this.compressInfo.outputFileInfo
      this.taskState = CompressTaskState.done;
    }
    if (this.compressInfo.state == CompressState.compressing) {
      this.taskState = CompressTaskState.excuting;
    }
    this.refreshInfos()
  }

  start(compressParams: any) {
    // 先加载压缩的信息
    this.compressParams = compressParams;
    this.node.startCompress(this.compressParams);
  }

  refreshInfos() {
    var fileSizeBytes = this.node.fileInfo.size;
    this.resolution = this.node.fileInfo.video_info.width + "x" + this.node.fileInfo.video_info.height;
    this.duration = this.formatTime(this.node.fileInfo.video_info.duration_ts)
    this.orgFileSizeStr = this.formatBytes(fileSizeBytes)
    this.orgBitRateStr = this.node.fileInfo.video_info.bit_rate
    if (this.compressInfo.state == CompressState.done) {
      console.log("what happen ????")
    }
    if (this.compressedFileInfo != null) {
      var compresscfileSizeBytes = this.compressedFileInfo.size;
      this.compressedFileSizeStr = this.formatBytes(compresscfileSizeBytes)
      this.compressedBitRateStr = this.compressedFileInfo.video_info.bit_rate
      this.progressStr = "100%"
    } else {
      if (this.compressInfo.state == CompressState.compressing) {
        // limit progrexx with xx.xx format
        this.progressStr = this.compressInfo.progress.toFixed(2) + "%"
      }
    }
    this.delegate?.taskInfoChanged(this);
    console.log("resolution ", this.resolution, "duration :", this.duration, "filesize:", this.orgFileSizeStr, "bitrate:", this.orgBitRateStr)
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

      var func = statusDealerMap.get(this.compressInfo.compressState);
      if (func) {
        func(this);
      }
    }
  }
}





export { CompressTask }