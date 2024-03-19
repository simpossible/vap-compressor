<template>
  <el-row style="height: 100%;">
    <el-col :span="12">
      <VapList />
    </el-col>
    <el-col :span="12">
      <VapDetail />
    </el-col>
  </el-row>

</template>
<script>
import VapList from "./view/list/list.vue";
import VapDetail from "./view/detail/detail.vue";

export default {
  components: {
    VapList,
    VapDetail
  },
  data() {
    return {
      count: 0
    }
  },
  methods: {
    increment() {
      this.count++
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