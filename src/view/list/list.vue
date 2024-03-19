<template>
  <div id="vap_list_area" :class="vap_list_style_class">
    这里是vap列表区域
    adf 
    adf
    adf
  </div>
</template>
<script>
export default {
  name: 'VapList',
  components: {

  },
  mounted() {
    console.log('the list area mounted')
    setTimeout(() => {
      console.log('add event listener')
      var element = document.getElementById("vap_list_area")
      element.addEventListener('drop', this.on_space_drop, false)
      element.addEventListener("dragenter", this.on_space_dragenter, false);

      element.addEventListener("dragover", this.on_space_dragover, false);

      element.addEventListener("dragleave", this.on_space_dragleave, false);
    }, 2000);
  },

  data() {
    return {
      vap_list_style_class: 'vap_list_style_normal',      
    };
  },
  methods: {
    on_space_dragenter(e) {
      e.preventDefault();
      e.stopPropagation();
    },
    on_space_dragover(e) {
      e.preventDefault();
      e.stopPropagation();
      this.change_list_area_hightlight(true)
    },
    on_space_dragleave(e) {
      e.preventDefault();
      e.stopPropagation();
      this.change_list_area_hightlight(false)
    },
    on_space_drop(e) {
      e.preventDefault();
      e.stopPropagation();
      this.change_list_area_hightlight(false)
      files = e.dataTransfer.files;
      console.log(files)
    },
    change_list_area_hightlight(hightlight = false) {
      this.vap_list_style_class = hightlight ? 'vap_list_style_highlight' : 'vap_list_style_normal'
    }    
  },
}
</script>

<style>
.vap_list_style_normal {
 
 background-color: red;
}
.vap_list_style_highlight {
  border-color: red;
  border-style: solid;
  background-color: yellow;
}
</style>