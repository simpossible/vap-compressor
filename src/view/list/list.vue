<template>
  <div>
    haha vaplist
  </div>
</template>
<script>
export default {
  name: 'VapList',
  components: {
      
  },

  data() {
    return {
    };
  },
  methods: {
    removeItem(index) {
      this.list.splice(index, 1);
    },
  },
}
</script>