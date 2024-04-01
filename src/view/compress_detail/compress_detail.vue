<template>
    <div v-if="node != null && node != undefined && node.src != ''">
        <div v-if="compressInfo.state == 0">
            <!-- 这里显示开始压缩按钮 -->
            <el-row style="margin-top: 100px;">
                <el-col :span="8"></el-col>
                <el-col :span="8"> <el-button @click="onCompressClicked" circle style="width: 100px;height: 100px;">
                        开始压缩
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
            <el-row>
                <!-- 这里是预览区域,区域的高度定为300px -->
                <div ref="anim" class="vap_anim">
                </div>
            </el-row>
            <el-row>
                <el-descriptions>
                    <el-descriptions-item label="分辨率">{{ resolution }}</el-descriptions-item>
                    <el-descriptions-item label="文件大小">{{ fileSize }}</el-descriptions-item>
                    <el-descriptions-item label="比特率">{{ bitRate }}</el-descriptions-item>
                    <el-descriptions-item label="时长">{{ duration }}</el-descriptions-item>
                </el-descriptions>
            </el-row>
            <el-row>
                <!-- 这个搞个文本区域来显示vapJson的参数,最高显示100px-->
                <el-col>
                </el-col>
                <el-col>
                    <el-button @click="quitCompress">放弃</el-button>
                </el-col>
                <el-col>
                    <el-button @click="acceptCompress">使用</el-button>
                </el-col>
                <el-col>
                </el-col>
            </el-row>
        </div>
    </div>

</template>
<script>
import { FileNode } from '../../sdk/file_node';
import Vap from 'video-animation-player';
import { vapUrlForKey, UrlPathDownload, UrlPathVapJson } from '../../sdk/url_config';

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
        console.log("this .node is ", this.node)
        this.node.addCompresseDelegate(this)
        this.compressInfo = this.node.compressInfo
        if (this.node.src != '') {
            this.node.loadCompressInfo()
        }
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
            compressNode: null,
            vap: null,
            fileUrl: "",
            vapJsonUrl: ""
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

        checkVapPlay() {
            if (this.compressInfo.state == 2 && this.fileUrl != "") {
                if (this.vap == null) {
                    this.play()
                }
            } else {
                if (this.vap != null) {
                    this.vap.pause()
                }
            }
        },

        onCompressClicked() {
            this.node.startCompress()
        },
        onNodeInfoLoaded(node) {
            console.log("onNodeInfoLoadedd");
            if (node == this.compressNode) {
                var fileSizeBytes = this.compressNode.fileInfo.size;
                this.fileSize = this.formatBytes(fileSizeBytes)
                this.resolution = this.compressNode.fileInfo.video_info.width + "x" + this.compressNode.fileInfo.video_info.height
                this.vapJson = JSON.stringify(this.compressNode.fileInfo.vap_info, null, 0)
                this.duration = this.formatTime(this.compressNode.fileInfo.video_info.duration_ts)
                this.bitRate = this.compressNode.fileInfo.video_info.bit_rate
                this.fileUrl = vapUrlForKey(UrlPathDownload, {path: this.compressNode.src});
                this.vapJsonUrl = vapUrlForKey(UrlPathVapJson, {path: this.compressNode.src});
                this.checkVapPlay()
            }
        },
        onNodeCompressInfoUpdated(node) {
            console.log('onNodeCompressInfoUpdated', node.compressInfo)
            if (node.src == this.node.src) {
                this.compressInfo = node.compressInfo;
                if (this.compressInfo.progress != undefined) {
                    this.progress = parseFloat(this.compressInfo.progress).toFixed(2);
                }
                if (this.compressInfo.state == 2) {
                    var targetFile = this.compressInfo.outputPath
                    if (this.compressNode == null) {
                        this.compressNode = new FileNode(targetFile)
                        this.compressNode.isOutputNode = true
                        this.compressNode.delegate = this
                        this.compressNode.initialData()
                    }
                }
                this.checkTimer()
            }
        },
        play() {
            console.log("start play vap");
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
                })
            window.vap = this.vap
        },
        pause() {
            if (this.vap != null) {
                this.vap.pause()
            }
        },
        playContinue() {
            if (this.vap != null) { 
            this.vap.play()
            }
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
        },
        quitCompress() {
            this.node.quitCompress()
        },
        acceptCompress() {
            this.node.acceptCompress()
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