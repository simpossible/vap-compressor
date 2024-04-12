import { FileNode } from "./file_node";
import { CompressState } from "../file_server/compress_state"
import { th } from "element-plus/es/locale";



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
  progress: number = 0; // 进度

  delegate: CompressTaskStateInterface | null = null;
  auto_accept: boolean = true;

  constructor(node: FileNode) {
    this.refreshKey = node.src
    this.node = node;
    this.compressInfo = this.node.compressInfo
    this.refreshInfos()
    this.node.addDelegates(this)
    this.node.addCompresseDelegate(this);
    this.taskState = CompressTaskState.preparing;
    console.log("发生了妹子")
    if (!this.node.initialed) {
      this.node.initialData()
    } else {
      this.node.loadCompressInfo();
    }

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
      if (this.taskState != CompressTaskState.done) {
        this.taskState = CompressTaskState.done
      }
    }
    console.log("gogogo", this.compressInfo)
    this.refreshInfos()
  }

  onNodeCompressCleared(node) {
    this.compressInfo = {};
    this.resolution = ""; // 分辨率
    this.duration = ""; // 时长
    this.compressParams = {}; // 压缩参数
    this.taskState = CompressTaskState.done; // 任务状态
    this.error = ""; // 错误信息  

    this.compressedFileInfo = null; // 压缩后的文件
    this.compressStateStr = ""; // 压缩状态
    this.orgFileSizeStr = ""; // 原始文件大小
    this.orgBitRateStr = ""; // 原始码率
    this.compressedFileSizeStr = ""; // 压缩后文件大小
    this.compressedBitRateStr = ""; // 压缩后码率
    this.displayPath = ""; // 显示路径
    this.refreshKey = ""; // 刷新时间
    this.progressStr = ""; // 进度
    this.progress = 0; // 进度
    this.delegate?.taskInfoChanged(this);
  }

  start(compressParams: any) {
    // 先加载压缩的信息
    compressParams.auto_accept = this.auto_accept;
    this.compressParams = compressParams;
    this.taskState = CompressTaskState.excuting
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
      if (this.compressInfo.errorMsg != undefined && this.compressInfo.errorMsg != "") {
        this.progressStr = this.compressInfo.errorMsg
      }
    } else {
      if (this.compressInfo.state == CompressState.compressing) {
        // limit progrexx with xx.xx format
        var progress = this.compressInfo.progress.toFixed(2)
        if (progress != undefined) {          
          this.progress = progress
          this.progressStr = progress + "%"
        }

      }
    }
    this.delegate?.taskInfoChanged(this);
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
  clear() {
    this.node.deleteDelegates(this)
    this.node.deleteCompresseDelegate(this)
    this.delegate = null
  }

}





export { CompressTask }