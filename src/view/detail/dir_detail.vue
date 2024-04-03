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
  </div>
</template>
<script>
import { FileNode } from '../../sdk/file_node';
import Vap from 'video-animation-player';
import { vapUrlForKey, UrlPathDownload, UrlPathVapJson } from '../../sdk/url_config';
import { CompressSpeedOptions, compressSpeedOptionDisplayName } from '../../sdk/compress_params';

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
  },
  unmounted() {
  },

  data() {
    return {
      compressQualityValue: 46, // 压缩质量
      compressQualityPercentage: '45%',
      compressSpeedValue: 4,
      compressSpeedMaxValue: CompressSpeedOptions.length - 1,
      compressSpeedTip: '',

    };
  },
  methods: {
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
    }
  }

}
</script>

<style>
</style>