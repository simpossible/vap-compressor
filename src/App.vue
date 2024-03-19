
<template>
  <el-button @click="increment">Click me: {{ count }}</el-button>
  <VapList />
</template>
<script>
import VapList from "./view/list/list.vue";
export default {
  components: {
    VapList
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
