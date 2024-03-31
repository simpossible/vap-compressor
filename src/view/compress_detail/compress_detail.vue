<template>
    <div v-if="compressInfo.state == 0">
        <!-- 这里显示开始压缩按钮 -->
        <el-row style="margin-top: 100px;">
            <el-col :span="8"></el-col>
            <el-col :span="8"> <el-button @click="onCompressClicked" circle style="width: 100px;height: 100px;"> 开始压缩
                </el-button></el-col>
            <el-col :span="8"></el-col>
        </el-row>
    </div>
    <div v-if="compressInfo.state == 1">
        <!-- 这里显示压缩进度 -->
        <el-row style="margin-top: 100px;">
            <el-col :span="8"></el-col>
            <el-col :span="8">
                <el-progress type="circle" :percentage="progress" :color="colors" :width="100" />
            </el-col>
            <el-col :span="8"></el-col>
        </el-row>
    </div>

    <div v-if="compressInfo.state == 2">
        <!-- 这里显示播放 -->
    </div>

</template>
<script>
import { FileNode } from '../../sdk/file_node';
import Vap from 'video-animation-player';

export default {
    name: 'VapCompressDetail',
    props: {
        node: {
            type: FileNode,
            required: true
        }
    },
    components: {

    },
    mounted() {
        this.node.addCompresseDelegate(this)
    },
    unmounted() {
        this.node.deleteCompresseDelegate(this)
    },

    data() {
        return {
            compressInfo: { state: 0 }, //当前的压缩信息
            progress: 0, // 压缩进度
            compressNode: null, // 压缩后的文件节点
            fileSize: "",
            resolution: "",
            vapJson: "",
            bitRate: "",
            duration: "",
            timer: null,
            colors: [
                { color: '#f56c6c', percentage: 20 },
                { color: '#e6a23c', percentage: 40 },
                { color: '#5cb87a', percentage: 60 },
                { color: '#1989fa', percentage: 80 },
                { color: '#6f7ad3', percentage: 100 },
            ],
        };
    },
    methods: {
        checkTimer() {
            if (this.compressInfo.state == 1) {
                if (this.timer == null) {
                    this.timer = setInterval(() => {
                        this.node.loadCompressInfo()
                    }, 1000)
                }
            } else {
                if (this.timer != null) {
                    clearInterval(this.timer)
                    this.timer = null
                }
            }
        },


        onCompressClicked() {
            this.node.startCompress()
        },
        onNodeCompressInfoUpdated(node) {
            console.log('onNodeCompressInfoUpdated', node.compressInfo)
            if (node.src == this.node.src) {
                this.compressInfo = node.compressInfo;
                if (this.compressInfo.progress != undefined) {
                    this.progress = parseFloat(this.compressInfo.progress).toFixed(2);
                }
                this.checkTimer()
            }
        },
        play() {
            const that = this
            var divWidth = this.$refs.anim.offsetWidth;
            var divHeight = this.$refs.anim.offsetHeight;
            this.vap = new Vap().play(Object.assign({}, {
                container: this.$refs.anim,
                // 素材视频链接
                src: this.fileUrl,
                // 素材配置json对象
                config: this.node.fileInfo.vap_info,
                width: divWidth,
                height: divHeight,
                // 同素材生成工具中配置的保持一致
                fps: 20,
                // 是否循环
                loop: true,
                // 起始播放时间点
                beginPoint: 0,
                // 精准模式
                accurate: true
                // 播放起始时间点(秒)
            }, { type: 1 }))
                .on('playing', () => {
                    console.log('playing')
                })
                .on('ended', () => {
                    console.log('ended')
                })
                .on('frame', (frame, timestamp) => {
                    // frame: 当前帧(从0开始)  timestamp: (播放时间戳)
                    console.log('frame', frame, timestamp)
                })
            window.vap = this.vap
        },
        pause() {
            this.vap.pause()
        },
        playContinue() {
            this.vap.play()
        },
        formatBytes(bytes) {
            if (bytes < 1024) {
                return bytes + " Bytes";
            } else if (bytes < 1048576) {
                return (bytes / 1024).toFixed(2) + " KB";
            } else {
                return (bytes / 1048576).toFixed(2) + " MB";
            }
        },
        formatTime(seconds) {
            let hours = Math.floor(seconds / 3600);
            let minutes = Math.floor((seconds % 3600) / 60);
            let secs = (seconds % 60).toFixed(1);

            let timeStr = "";

            if (hours > 0) {
                timeStr += hours + "小时";
            }
            if (minutes > 0) {
                timeStr += minutes + "分钟";
            }
            if (secs > 0) {
                timeStr += secs + "秒";
            }

            return timeStr;
        }
    }

}
</script>

<style>
.vap_anim {
    width: 37.5vw;
    height: 37.5vw;
}
</style>