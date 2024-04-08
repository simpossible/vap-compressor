<template>
  <div v-if="node.src.length != 0" style="background-color: aqua;">
    <!-- 这里显示开始压缩按钮 -->
    <el-row style="margin-top: 100px;">
      <el-col :span="8"></el-col>
      <el-col :span="8"> <el-button @click="onCompressClicked" circle style="width: 100px;height: 100px;">
          开始压缩
        </el-button></el-col>
      <el-col :span="8"></el-col>
    </el-row>
    <!-- 加一个压缩质量的可选进度条 -->
    <el-row style="background-color: antiquewhite;">
      <el-col :span="1"></el-col>
      <el-col :span="5" style="margin-top: 5px;"><el-text class="mx-1">压缩质量</el-text></el-col>
      <el-col :span="14">
        <el-slider v-model="compressQualityValue" :step="1" :max="102" :format-tooltip="qualityTip"
          @change="onCompressQualityChange" />
      </el-col>
      <el-col :span="3" style="margin-top: 5px;padding-left: 6px;"> <el-text class="mx-1" size="small">{{
    compressQualityPercentage }}</el-text> </el-col>
    </el-row>
    <!-- 加一个压缩速度的可选进度条 -->
    <el-row style="background-color:brown;">
      <el-col :span="1"></el-col>
      <el-col :span="5" style="margin-top: 5px;"><el-text class="mx-1">压缩速度</el-text></el-col>
      <el-col :span="14">
        <el-slider v-model="compressSpeedValue" :step="1" :max="compressSpeedMaxValue" :format-tooltip="speedQualityTip"
          @change="onCompressSpeedQualityChange" />
      </el-col>
      <el-col :span="3" style="margin-top: 5px;padding-left: 6px;"> <el-text class="mx-1" size="small">{{
    compressSpeedTip }}</el-text> </el-col>
    </el-row>
    <el-row>
      <!-- 这里加一个列表用来显示所有的压缩任务的进度 -->
      <el-table :data="taskList" style="width: 100%">
        <el-table-column prop="displayPath" label="路径">
        </el-table-column>
        <el-table-column prop="resolution" label="分辨率">
        </el-table-column>
        <el-table-column prop="duration" label="时长">
        </el-table-column>
        <el-table-column prop="orgFileSizeStr" label="原始大小">
        </el-table-column>
        <el-table-column prop="orgBitRateStr" label="原始码率">
        </el-table-column>
        <el-table-column prop="compressedFileSizeStr" label="输出大小">
        </el-table-column>
        <el-table-column prop="compressedBitRateStr" label="输出码率">
        </el-table-column>
      </el-table>
    </el-row>
  </div>
</template>
<script>
import { FileNode } from '../../sdk/file_node';
import { CompressTask } from '../../sdk/compress_task';
import Vap from 'video-animation-player';
import { vapUrlForKey, UrlPathDownload, UrlPathVapJson } from '../../sdk/url_config';
import { CompressSpeedOptions, compressSpeedOptionDisplayName } from '../../sdk/compress_params';
import { CompressState } from '../../file_server/compress_state';
import { setTransitionHooks } from 'vue';

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
    this.taskList = []

  },

  data() {
    return {
      compressQualityValue: 46, // 压缩质量
      compressQualityPercentage: '45%',
      compressSpeedValue: 4,
      compressSpeedMaxValue: CompressSpeedOptions.length - 1,
      compressSpeedTip: '',
      taskList: []
    };
  },
  methods: {
    loadAllTask() {
      // 加载所有的压缩任务
      console.log('load all task')
      this.node.getVapList().then((vapList) => {
        var tempTaskList = []
        for (node of vapList) {
          var task = new CompressTask(node)
          task.displayPath = node.src.replace(this.node.src, '')
          task.delegate = this
          tempTaskList.push(task)
        }
        this.taskList = tempTaskList
      }).catch((error) => {
        console.log('load all task error:', error)
      })
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
      this.startCompress()
    },

    startCompress() {
      // 开始压缩任务
      var alreadyFinish = true
      for (task of this.taskList) {
        if (task.status == CompressState.none) {
          task.start({
            quality: this.compressQualityValue / 2,
            speed: CompressSpeedOptions[this.compressSpeedValue]
          })
          alreadyFinish = false
          break
        }
      }
      if (alreadyFinish) {
        this.$message({
          message: '所有任务已经完成',
          type: 'warning'
        });
      }
    },

    taskStateChanged(task) {
      this.startCompress()
    }
  }

}
</script>

<style></style>