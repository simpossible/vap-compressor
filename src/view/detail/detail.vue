<template>
    <div v-if="node.src.length != 0" id="vap_detail_area">
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
            <el-col :span="24">
            <el-button @click="play" style="margin: 6px 6px 6px 6px;">压缩</el-button>
            </el-col>
        </el-row>
        <el-row>
            <!-- 这个搞个文本区域来显示vapJson的参数,最高显示100px-->
            <el-input type="textarea" :rows="5" :autosize="{ minRows: 5, maxRows: 5 }" :value="vapJson" readonly>
            </el-input>
        </el-row>
    </div>
</template>
<script>
import { FileNode } from '../../sdk/file_node';
import Vap from 'video-animation-player';

export default {
    name: 'VapDetail',
    props: {
        node: {
            type: FileNode,
            required: true
        }
    },
    components: {

    },
    mounted() {
        if (this.node != null & this.node != undefined & this.node.src.length > 0) {
            console.log('the detail area mounted', this.node)
            // filesize to xx MB
            //              file_info: {
            //     size: 905250,
            //     video_info: {
            //     codec_name: 'h264',
            //     width: 1136,
            //     height: 1632,
            //     duration_ts: '1.520000',
            //     bit_rate: '4708400'
            //     },
            //     vap_info: { info: [Object], src: [Array], frame: [Array] }
            // },
            var fileSizeBytes = this.node.fileInfo.size;
            this.fileSize = this.formatBytes(fileSizeBytes)
            this.resolution = this.node.fileInfo.video_info.width + "x" + this.node.fileInfo.video_info.height
            this.vapJson = JSON.stringify(this.node.fileInfo.vap_info, null, 0)
            this.duration = this.formatTime(this.node.fileInfo.video_info.duration_ts)
            this.bitRate = this.node.fileInfo.video_info.bit_rate
            this.fileUrl = "http://127.0.0.1:3000/download?path=" + this.node.src
            this.vapJsonUrl = "http://127.0.0.1:3000/vap-json?vap-path=" + this.node.src
            this.play()
        }
    },
    unmounted() {
        if (this.vap != null) {
            this.vap.pause()
        }        
    },

    data() {
        return {
            fileSize: "",
            resolution: "",
            vapJson: "",
            bitRate: "",
            duration: ""
        };
    },
    methods: {
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
            },  { type: 1 }))
                .on('playing', () => {
                    console.log('playing')
                })
                .on('ended', () => {
                    console.log('ended')
                })
                .on('frame', (frame, timestamp) => {
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