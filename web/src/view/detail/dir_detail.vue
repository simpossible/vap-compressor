<template>
  <div v-if="node.src.length != 0" style="position: relative; height: 100%;">
    <!-- 这里显示开始压缩按钮 -->
    <el-row v-if="started === false" style="padding-top: 64px;">
      <el-col :span="8"></el-col>
      <el-col :span="8"> <el-button @click="onCompressClicked" circle style="width: 100px;height: 100px;">
          开始压缩
        </el-button></el-col>
      <el-col :span="8"></el-col>
    </el-row>
    <!-- 加一个压缩质量的可选进度条 -->
    <el-row v-if="started === false" style="margin-top: 20px;">
      <el-col :span="1"></el-col>
      <el-col :span="5" style="margin-top: 5px; width: 60px; max-width: 60px;"><el-text
          class="mx-1">压缩质量</el-text></el-col>
      <el-col :span="14">
        <el-slider v-model="compressQualityValue" :step="1" :max="102" :format-tooltip="qualityTip"
          @change="onCompressQualityChange" />
      </el-col>
      <el-col :span="3" style="margin-top: 5px;padding-left: 6px;"> <el-text class="mx-1" size="small">{{
    compressQualityPercentage }}</el-text> </el-col>
    </el-row>
    <!-- 加一个压缩速度的可选进度条 -->
    <el-row v-if="started === false" style="margin-top: 20px;">
      <el-col :span="1"></el-col>
      <el-col :span="5" style="margin-top: 5px; max-width: 60px;"><el-text class="mx-1">压缩速度</el-text></el-col>
      <el-col :span="14">
        <el-slider v-model="compressSpeedValue" :step="1" :max="compressSpeedMaxValue" :format-tooltip="speedQualityTip"
          @change="onCompressSpeedQualityChange" />
      </el-col>
      <el-col :span="3" style="margin-top: 5px;padding-left: 6px;"> <el-text class="mx-1" size="small">{{
    compressSpeedTip }}</el-text> </el-col>
    </el-row>
    <el-row v-if="started === false">
      <el-col :span="1"></el-col>
      <el-col :span="22">
        <el-text class="mx-1" size="small" style="margin-top: 6px;">Tip: 压缩速度约快 压缩率越低。压缩质量越大，耗时就越久</el-text>
      </el-col>
    </el-row>
    <el-row>
      <!-- 这里加一个列表用来显示 所有的压缩任务的进度 -->
      <el-table :data="taskList" :key="refreshKey" style="width: 100%">
        <el-table-column label="路径">
          <template #default="scope">
            <el-text class="mx-1" size="small">
              {{ scope.row.displayPath }}
            </el-text>
          </template>
        </el-table-column>
        <el-table-column align="center" label="分辨率">
          <template #default="scope">
            <el-text class="mx-1" size="small">
              {{ scope.row.resolution }}
            </el-text>
          </template>
        </el-table-column>
        <el-table-column prop="duration" label="时长">
          <template #default="scope">
            <el-text class="mx-1" size="small">
              {{ scope.row.duration }}
            </el-text>
          </template>
        </el-table-column>
        <el-table-column prop="orgFileSizeStr" label="原始大小">
        </el-table-column>
        <el-table-column prop="orgBitRateStr" label="原始码率">
        </el-table-column>
        <el-table-column prop="compressedFileSizeStr" label="输出大小">
        </el-table-column>
        <el-table-column prop="compressedBitRateStr" label="输出码率">
        </el-table-column>
        <el-table-column prop="progressStr" label="压缩进度">
        </el-table-column>
      </el-table>
    </el-row>
    <div v-if="finished == true"
      style="position: absolute; top: 0; bottom: 0; left: 0; right: 0; background-color: rgba(0, 0, 0, 0.3); z-index: 999; display: flex; justify-content: center; align-items: center;">
      <!-- 盖一个半透明的的上面。加个按钮 -->
      <div>
        <!-- BEGIN: ed8c6549bwf9 -->
        <el-button @click="clearButtonClicked" circle style="width: 100px;height: 100px;">clear</el-button>
        <!-- END: ed8c6549bwf9 -->
      </div>
    </div>
  </div>
</template>
<script>
import { FileNode } from '../../sdk/file_node';
import { CompressTask, CompressTaskState } from '../../sdk/compress_task';
import Vap from 'video-animation-player';
import { vapUrlForKey, UrlPathDownload, UrlPathVapJson } from '../../sdk/url_config';
import { CompressSpeedOptions, compressSpeedOptionDisplayName } from '../../sdk/compress_params';
import { shared_center } from '../../sdk/vap_center';

export default {
  name: 'DirDetail',
  props: {
    node: {
      type: FileNode,
      required: true
    }
  },
  components: {

  },
  mounted() {
    this.loadAllTask()
  },
  unmounted() {
    for (let task of this.taskList) {
      task.clear()
    }
    this.taskList = []

  },

  data() {
    return {
      compressQualityValue: 46, // 压缩质量
      compressQualityPercentage: '45%',
      compressSpeedValue: 4,
      compressSpeedMaxValue: CompressSpeedOptions.length - 1,
      compressSpeedTip: '',
      taskList: [],
      refreshKey: "",
      needUpdate: false, //避免频繁刷新。刷新频率yis
      started: false, //  是否已经开始了
      finished: false
    };
  },
  methods: {
    loadAllTask() {
      // 加载所有的压缩任务
      console.log('load all task')
      this.node.getVapList().then((vapList) => {
        var tempTaskList = []
        for (var node of vapList) {
          var task = new CompressTask(node)
          task.displayPath = node.src.replace(this.node.src, '')
          task.delegate = this
          tempTaskList.push(task)
        }
        this.taskList = tempTaskList
        this.checkfinish()
      }).catch((error) => {
        console.log('load all task error:', error)
      })
    },
    taskInfoChanged(task) {
      console.log('taskInfoChanged:', task.taskState)
      if (task.taskState == CompressTaskState.done) {
        if (this.started == true) {
          this.startCompress()
        } else {
          this.checkfinish()
        }
      } else {
      }
      this.needUpdate = true;
      // update after 1s
      setTimeout(() => {
        this.checkListUpdate()
      }, 160)
    },
    checkListUpdate() {
      if (this.needUpdate) {
        console.log("want to update list")
        this.needUpdate = false
        this.refreshKey = "" + (new Date().getTime())
        this.taskList = this.taskList
      }
    },
    qualityTip(number) {
      return (number * 100 / 102).toFixed(2) + "%";
    },
    onCompressQualityChange() {
      this.compressQualityPercentage = (this.compressQualityValue * 100 / 102).toFixed(2) + "%";
    },
    speedQualityTip(number) {
      var value = CompressSpeedOptions[number];
      return compressSpeedOptionDisplayName(value);
    },
    onCompressSpeedQualityChange() {
      var value = CompressSpeedOptions[this.compressSpeedValue];
      this.compressSpeedTip = compressSpeedOptionDisplayName(value);
    },
    onCompressClicked() {
      this.started = true
      this.startCompress()
      shared_center.dealingNodeSrc = this.node.src
    },

    startCompress() {
      console.log("go to compress")
      // 开始压缩任务      
      var alreadyFinish = true
      for (let task of this.taskList) {
        if (task.taskState == CompressTaskState.prepaired) {
          task.start({
            quality: (51 - this.compressQualityValue / 2),
            speed: CompressSpeedOptions[this.compressSpeedValue]
          })
          alreadyFinish = false
          break
        }
      }
      if (alreadyFinish && this.finished == false) {
        this.dealFinish()
        this.$message({
          message: '所有任务已经完成',
          type: 'warning'
        });
      }
    },

    dealFinish() {
      this.finished = true
      if (shared_center.dealingNodeSrc === this.node.src) {
        shared_center.dealingNodeSrc = ""
      }
    },

    checkfinish() {
      console.log("task checkfinish")
      var alreadyFinish = true
      for (let task of this.taskList) {
        if (task.taskState != CompressTaskState.done) {
          alreadyFinish = false
          break
        }
      }
      if (alreadyFinish && this.finished == false) {
        this.dealFinish()
        this.$message({
          message: '所有任务已经完成',
          type: 'warning'
        });
      }
    },
    clearButtonClicked() {
      console.log("ready to clear all task")
      var pathList = []
      for (let task of this.taskList) {
        pathList.push(task.node.src)
      }

      shared_center.clearCompressInfoForFiles(pathList).then(() => {
        for (let task of this.taskList) {
          task.clear()
          task.node.triggerCompressCleared()

        }
        this.loadAllTask()
        this.finished = false
      }).catch((e) => {
        console.log(e)
      })
    },
    taskStateChanged(task) {
      this.startCompress()
    }
  }

}
</script>

<style></style>