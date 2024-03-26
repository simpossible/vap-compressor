<template>
  <el-row style="height: 100%;width: 100%;">
    <el-col :span="6">
      <VapList :onVapChoosed="onListVapChoosed"/>
    </el-col>
    <el-col :span="9">
      <VapDetail :key="refreshKey" :node="selectNode" ></VapDetail>
    </el-col>
    <el-col :span="9">

    </el-col>
  </el-row>

</template>
<script>
import VapList from "./view/list/list.vue";
import VapDetail from "./view/detail/detail.vue";
import { FileNode } from "./sdk/file_node";

export default {
  components: {
    VapList,
    VapDetail
  },
  data() {
    return {
      count: 0,
      selectNode:new FileNode(""),
      refreshKey: 0
    }
  },
  methods: {
    increment() {
      this.count++
    },
    onListVapChoosed(node){
      console.log("listVapChoosed", node)
      this.refreshKey = this.refreshKey + 1
      this.selectNode = node
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