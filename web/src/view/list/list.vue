<template>
  <div id="vap_list_area" :class="vap_list_style_class" style="position: relative;">
    <el-tree v-if="vm_list.length > 0" style="max-width: 600px" ref="vap_list_tree" :data="vm_list" node-key="uuid"
      :props="defaultProps" @node-click="handleNodeClick">
    </el-tree>
    <div v-if="vm_list.length === 0"
      style="position: absolute; top: 0; bottom: 0; left: 0; right: 0;  display: flex; justify-content: center; align-items: center;">
      <el-text style="color: white;">将文件或者文件夹拖入这里</el-text>
    </div>
  </div>

</template>
<script>
import { shared_center } from '../../sdk/vap_center';
import CellNodeVm from './cell_node_vm';

export default {
  name: 'VapList',
  props: {
    onVapChoosed: {
      type: Function,
      required: true
    }
  },
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
    var node_list = shared_center.nodes;
    for (var i = 0; i < node_list.length; i++) {
      let node = node_list[i]
      var vm = new CellNodeVm(node)
      this.vm_list_map.set(node.src, vm);
      this.vm_list.push(vm)
    }
    shared_center.delegate = this
  },

  data() {
    return {
      vap_list_style_class: 'vap_list_style_normal',
      vm_list: [],
      vm_list_map: new Map(),
      defaultProps: {
        children: 'children',
        label: 'label',
      },
      selectNode: null
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
      let files = e.dataTransfer.files;
      console.log(files)
      shared_center.createFileNodeFromFiles(files);

    },
    change_list_area_hightlight(hightlight = false) {
      this.vap_list_style_class = hightlight ? 'vap_list_style_highlight' : 'vap_list_style_normal'
    },
    onNodeAdded(node) {
      console.log('onNodeAdded', node)
      if (this.vm_list_map.has(node.src)) {
        console.log("node exist");
        return
      }
      var shouldAutoChoose = this.vm_list.length === 0
      var vm = new CellNodeVm(node)
      this.vm_list.unshift(vm)
      this.vm_list_map.set(node.src, vm)
      if (this.$refs.vap_list_tree != undefined) {
        this.$refs.vap_list_tree.updateKeyChildren(node.src, this.vm_list);
      }

      if (shouldAutoChoose) {
        this.onVapChoosed(node)
      }

    },
    handleNodeClick(ele, nodeinfo, treeNode, e) {
      console.log("on node clicked")
      ele.reloadFiles();
      this.onVapChoosed(ele.node);
    }
  },
}
</script>

<style>
.vap_list_style_normal {
  height: 100%;
  background-color: #79bbff;
}

.vap_list_style_highlight {
  height: 100%;
  border-style: solid;
}
</style>