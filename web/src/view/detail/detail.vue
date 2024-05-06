<template>
    <div v-if="node.src.length != 0" id="vap_detail_area">
        <el-row>
            <!-- 这里是预览区域,区域的高度定为300px -->
            <div ref="anim" class="vap_anim">
            </div>
        </el-row>
        <el-row>
            <el-table :data="tableData">
                <el-table-column prop="resolution" label="分辨率"></el-table-column>
                <el-table-column prop="fileSize" label="文件大小"></el-table-column>
                <el-table-column prop="bitRate" label="比特率"></el-table-column>
                <el-table-column prop="duration" label="时长"></el-table-column>
            </el-table>
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
import { vapUrlForKey, UrlPathDownload, UrlPathVapJson } from '../../sdk/url_config';

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
            this.node.addDelegates(this)
            this.refreshInfo()
            this.play()
        }
    },
    unmounted() {
        this.node.deleteDelegates(this)
        if (this.vap != null) {
            this.vap.pause()
            this.vap = null
        }
    },

    data() {
        return {
            fileSize: "",
            resolution: "",
            vapJson: "",
            bitRate: "",
            duration: "",
            tableData: []
        };
    },
    methods: {
        onNodeInfoLoaded(node) {
            if (node == this.node) {
                var shouldRefreshPlay = false
                if (this.fileSize != this.formatBytes(node.fileInfo.size)) {
                    this.fileSize = this.formatBytes(node.fileInfo.size)
                    shouldRefreshPlay = true
                }
                this.refreshInfo()
                if (shouldRefreshPlay) {
                    this.stop()
                    this.play()
                }
            }
        },
        refreshInfo() {
            var fileSizeBytes = this.node.fileInfo.size;
            this.fileSize = this.formatBytes(fileSizeBytes)
            this.resolution = this.node.fileInfo.video_info.width + "x" + this.node.fileInfo.video_info.height
            this.vapJson = JSON.stringify(this.node.fileInfo.vap_info, null, 0)
            this.duration = this.formatTime(this.node.fileInfo.video_info.duration_ts)
            this.bitRate = this.node.fileInfo.video_info.bit_rate
            this.fileUrl = vapUrlForKey(UrlPathDownload, { path: this.node.src });
            this.vapJsonUrl = vapUrlForKey(UrlPathVapJson, { path: this.node.src });
            this.tableData = [{
                resolution: this.resolution,
                fileSize: this.fileSize,
                bitRate: this.bitRate,
                duration: this.duration
            }]
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

                })
                .on('ended', () => {
                })
                .on('frame', (frame, timestamp) => {
                })
        },
        pause() {
            this.vap.pause()
        },
        stop() {
            if (this.vap != null) {
                this.vap.stop()
                this.vap.destroy()
                this.vap = null;
            }
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