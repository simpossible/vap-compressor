<template>
  <div style="height: 100%;width: 100%;">
    <el-row style=" height: 100%;width: 100%;">
      <el-col :span="6" id="xx1">
        <VapList :onVapChoosed="onListVapChoosed" />
      </el-col>
      <el-col :span="9" v-if="selectNode.fileType == 2">
        <VapDetail :key="refreshKey" :node="selectNode"></VapDetail>
      </el-col>
      <el-col :span="9" v-if="selectNode.fileType == 2">
        <VapCompressDetail :key="refreshKey" :node="selectNode"> </VapCompressDetail>
      </el-col>
      <el-col :span="18" v-if="selectNode.fileType == 1">
        <DirDetail :key="selectNode.src" :node="selectNode"></DirDetail>
      </el-col>

    </el-row>

  </div>

</template>
<script>
import VapList from "./view/list/list.vue";
import VapDetail from "./view/detail/detail.vue";
import VapCompressDetail from "./view/compress_detail/compress_detail.vue";
import { FileNode } from "./sdk/file_node";
import DirDetail from "./view/detail/dir_detail.vue";
import { shared_center } from "./sdk/vap_center";

export default {
  components: {
    VapList,
    VapDetail,
    VapCompressDetail,
    DirDetail
  },
  data() {
    return {
      count: 0,
      selectNode: new FileNode(""),
      refreshKey: 0,
      waitingSrc: "",
      dialogVisible: false

    }
  },
  methods: {
    increment() {
      this.count++
    },
    onListVapChoosed(node) {
      console.log("onListVapChoosed", this.selectNode);
      if (shared_center.dealingNodeSrc != "" && shared_center.dealingNodeSrc != node.src) {
        this.$message({
          message: '任务正在处理中',
          type: 'warning'
        });
        return
      }
      console.log("listVapChoosed", node)
      if (this.selectNode != null && this.selectNode.src != node.src) {
        this.selectNode.deleteDelegates(this)
      }
      if (node.initialed) {
        this.selectNode = node
        this.waitingSrc = ""
        node.addDelegates(this);
        this.refreshKey = this.refreshKey + 1
      } else {
        this.waitingSrc = node.src
        this.selectNode = new FileNode("")
        node.addDelegates(this);
      }
      this.checkAlert();


    },
    onNodeInfoLoaded(node) {
      console.log("onNodeInfoLoaded", this.selectNode);
      if (node.src == this.waitingSrc) {
        this.refreshKey = this.refreshKey + 1
        this.selectNode = node
        this.waitingSrc = ""
        node.deleteDelegates(this);
        this.checkAlert();
      }
      node.deleteDelegates(this);
    },
    checkAlert() {
      console.log("checkAlert", this.selectNode);
      if (this.waitingSrc != "") {
        return;
      }
      if (this.selectNode.fileType == 2 || this.selectNode.fileType == 1) {
        return;
      }
      this.$alert('<span>请拖入正确的VAP特效文件或者文件夹</span><br/><a style="fontSize:8;color:blue" href="https://github.com/Tencent/vap">了解vap</a>', '文件不支持', {
        dangerouslyUseHTMLString: true
      });
    },
    handleClose(done) {
      console.log("haha done");
      done();
    }

  },

  mounted() {
    console.log(`The initial count is ${this.count}.`)
  }
}
</script>
<style>
body {
  margin: 0rem;
  padding: 0rem;
}

.app {
  height: 100%;
}
</style>