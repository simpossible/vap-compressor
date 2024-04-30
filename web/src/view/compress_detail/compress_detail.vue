<template>
    <div v-if="node != null && node != undefined && node.src != '' && task != null">
        <div v-if="task.taskState == 2">
            <!-- 这里显示开始压缩按钮 -->
            <el-row style="margin-top: 100px;">
                <el-col :span="8"></el-col>
                <el-col :span="8"> <el-button @click="onCompressClicked" circle style="width: 100px;height: 100px;">
                        开始压缩
                    </el-button></el-col>
                <el-col :span="8"></el-col>
            </el-row>
            <!-- 加一个压缩质量的可选进度条 -->
            <el-row style="margin-top: 12px;">
                <el-col :span="1"></el-col>
                <el-col :span="5" style="margin-top: 5px; width: 60px; max-width: 60px;"><el-text
                        class="mx-1">压缩质量</el-text></el-col>
                <el-col :span="14">
                    <el-slider v-model="compressQualityValue" :step="1" :max="102" :format-tooltip="qualityTip"
                        @change="onCompressQualityChange" />
                </el-col>
                <el-col :span="3" style="margin-top: 5px;padding-left: 6px;"> <el-text class="mx-1" size="small">{{
        compressQualityPercentage }}</el-text> </el-col>
            </el-row>
            <!-- 加一个压缩速度的可选进度条 -->
            <el-row style="margin-top: 12px;">
                <el-col :span="1"></el-col>
                <el-col :span="5" style="margin-top: 5px; max-width: 60px;"><el-text
                        class="mx-1">压缩速度</el-text></el-col>
                <el-col :span="14">
                    <el-slider v-model="compressSpeedValue" :step="1" :max="compressSpeedMaxValue"
                        :format-tooltip="speedQualityTip" @change="onCompressSpeedQualityChange" />
                </el-col>
                <el-col :span="4" style="margin-top: 5px; padding-left: 12px;">
                    <el-text class="mx-1" size="small">
                        {{ compressSpeedTip }}
                    </el-text>
                </el-col>

            </el-row>
            <el-row>
                <el-col style="margin-left: 12px;">
                    <el-text class="mx-1" size="small" style="margin-top: 6px;">Tip: 压缩速度约快 压缩率越低。压缩质量越大，耗时就越久</el-text>
                </el-col>
            </el-row>
        </div>
        <div v-if="task.taskState == 3">
            <!-- 这里显示压缩进度 -->
            <el-row style="margin-top: 100px;">
                <el-col :span="8"></el-col>
                <el-col :span="8">
                    <el-progress type="circle" :percentage="task.progress" :color="colors" :width="100" />
                </el-col>
                <el-col :span="8"></el-col>
            </el-row>
        </div>

        <div v-if="canPreview === true">
            <!-- 这里显示播放 -->
            <el-row>
                <!-- 这里是预览区域,区域的高度定为300px -->
                <div ref="anim_area" class="vap_anim">
                </div>
            </el-row>
            <el-row>
                <el-table :data="tableData">
                    <el-table-column prop="resolution" label="分辨率"></el-table-column>
                    <el-table-column prop="compressedFileSizeStr" label="文件大小"></el-table-column>
                    <el-table-column prop="compressedBitRateStr" label="比特率"></el-table-column>
                    <el-table-column prop="duration" label="时长"></el-table-column>
                </el-table>
            </el-row>
            <el-row style="margin-top: 8px;">
                <!-- 这个搞个文本区域来显示vapJson的参数,最高显示100px-->
                <el-col :span="4"></el-col>
                <el-col :span="7">
                    <el-button @click="quitCompress">放弃</el-button>
                </el-col>
                <el-col :span="2"></el-col>
                <el-col :span="7">
                    <el-button @click="acceptCompress">使用</el-button>
                </el-col>
                <el-col :span="4"></el-col>
            </el-row>
        </div>
    </div>

</template>
<script>
import { FileNode } from '../../sdk/file_node';
import Vap from 'video-animation-player';
import { vapUrlForKey, UrlPathDownload, UrlPathVapJson } from '../../sdk/url_config';
import { CompressSpeedOptions, compressSpeedOptionDisplayName } from '../../sdk/compress_params';
import { CompressTask, CompressTaskState } from '../../sdk/compress_task'
import { shared_center } from '../../sdk/vap_center'

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
        this.refreshTask()
    },

    unmounted() {
        this.node.deleteCompresseDelegate(this)
    },

    data() {
        return {
            task: null,
            compressQualityValue: 46, // 压缩质量
            compressQualityPercentage: '45%',
            compressSpeedValue: 4,
            compressSpeedMaxValue: CompressSpeedOptions.length - 1,
            compressSpeedTip: '',
            colors: [
                { color: '#f56c6c', percentage: 20 },
                { color: '#e6a23c', percentage: 40 },
                { color: '#5cb87a', percentage: 60 },
                { color: '#1989fa', percentage: 80 },
                { color: '#6f7ad3', percentage: 100 },
            ],
            vap: null,
            fileUrl: "",
            vapJsonUrl: "",
            tableData: [],
            canPreview: false,
            isOperated: false
        };
    },
    methods: {
        refreshTask() {
            this.canPreview = false
            this.isOperated = false
            this.fileUrl = ""
            this.vapJsonUrl = ""
            this.stopPlay()
            if (this.node.src != '') {
                this.task = new CompressTask(this.node)
                this.task.auto_accept = false
                this.task.delegate = this
                this.tableData = [this.task]
                this.onCompressSpeedQualityChange()
            }
        },
        taskStateChanged() {

        },
        taskInfoChanged(task) {
            console.log("taskInfoChanged:state", task.taskState)
            if (task.taskState == CompressTaskState.done) {
                shared_center.dealingNodeSrc = ''
                if (task.compressedFileInfo != null) {
                    if (task.compressedFileInfo.video_info != undefined && task.compressedFileInfo.video_info != null) {
                        if (this.canPreview === false && this.isOperated === false) {
                            this.canPreview = true
                            var targetFile = task.compressedFileInfo.path
                            this.fileUrl = vapUrlForKey(UrlPathDownload, { path: targetFile });
                            this.vapJsonUrl = vapUrlForKey(UrlPathVapJson, { path: targetFile });
                            setTimeout(() => {
                                this.checkVapPlay()
                            }, 1000);
                        }
                    }

                } else {
                    console.log("task.compressedFileInfo is null")
                }
            }
        },


        checkVapPlay() {
            if (this.vap == null) {
                this.play()
            }
        },

        onCompressClicked() {
            if (this.task.taskState == CompressTaskState.prepaired) {
                shared_center.dealingNodeSrc = this.node.src
                this.task.start({
                    quality: this.compressQualityValue / 2,
                    speed: CompressSpeedOptions[this.compressSpeedValue]
                })
            }
        },
        play() {
            console.log("start play vap");
            const that = this
            var divWidth = this.$refs.anim_area.offsetWidth;
            console.log("start play vap: w ", divWidth);
            var divHeight = this.$refs.anim_area.offsetHeight;
            console.log("start play vap: h ", divHeight);
            this.vap = new Vap().play(Object.assign({}, {
                container: this.$refs.anim_area,
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
        stopPlay() {
            if (this.vap != null) {
                this.vap = null
            }
        },
        quitCompress() {
            this.task.clear()
            this.canPreview = false
            this.isOperated = true
            this.stopPlay()
            var that = this
            this.node.quitCompress((code, error) => {
                that.node.triggerCompressCleared()
                if (code == 0) {
                    this.refreshTask()
                } else {
                    this.$message({
                        message: error,
                        type: 'warning'
                    });
                }
            })
        },
        acceptCompress() {
            this.task.clear()
            this.canPreview = false
            this.isOperated = true
            this.stopPlay()
            this.node.acceptCompress((code, error) => {
                this.node.triggerCompressCleared()
                if (code == 0) {
                    this.refreshTask()
                } else {
                    this.$message({
                        message: error,
                        type: 'warning'
                    });
                }
            })
        },
        qualityTip(number) {
            return (number * 100 / 102).toFixed(2) + "%";
        },
        onCompressQualityChange() {
            this.compressQualityPercentage = (this.compressQualityValue * 100 / 102).toFixed(2) + "%";
        },
        speedQualityTip(number) {
            var value = CompressSpeedOptions[number];
            return compressSpeedOptionDisplayName(value);
        },
        onCompressSpeedQualityChange() {
            var value = CompressSpeedOptions[this.compressSpeedValue];
            this.compressSpeedTip = compressSpeedOptionDisplayName(value);
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