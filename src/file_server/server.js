const http = require('http')
const fs = require('fs')

const path = require('path')

import { getVapBoxes, addVapInfoToMp4, getFileInfoOfVap, getVapInfo, ffprobe, ffprobeStatic } from './vap_parser.js'
import { CompressState } from './compress_state.js'
// md文件访问在electron完全不行。搞个服务来做吧。

var codeOk = 666


// 压缩信息的字典。存储的是路径和压缩信息的关系
var compressInfoMap = new Map()

var server_map = new Map()

function isFileInnerPath(fileName) {
    // 是否是临时文件
    if (fileName.startsWith("__compress_")) {
        return true
    }
    return false
}



async function onFileRequest(req, params, res) {

    var filePath = params.get("path")
    var fileExist = fs.existsSync(filePath);
    var result = {}
    if (!fileExist) {
        console.log("onFileRequest 1")
        result = {
            "code": -1,
            "msg": "file not exist",
            "file_info": {},
            "is_dir": false,
            "is_vap": false
        }
        res.writeHead(200, { 'Content-Type': 'application/jsonn' })
        res.end(JSON.stringify(result))
        return
    }
    var fileStat = fs.statSync(filePath)
    var isDir = fileStat.isDirectory()
    var isVap = false
    var file_info = {}
    if (!isDir) {
        file_info["size"] = fileStat.size;
        result["sub_files"] = []
        // is mp4
        if (filePath.endsWith(".mp4")) {
            var vapJson = getVapInfo(filePath);
            if (vapJson != null) {
                const fileMetaData = await ffprobe(filePath, { path: ffprobeStatic.path })
                if (fileMetaData.streams.length > 0) {
                    for (var info in fileMetaData.streams) {
                        var stream = fileMetaData.streams[info]
                        if (stream.codec_type == "video") {
                            file_info["video_info"] = {
                                "codec_name": stream.codec_name,
                                "width": stream.width,
                                "height": stream.height,
                                "duration_ts": stream.duration,
                                "bit_rate": stream.bit_rate,
                            }
                            break;
                        }
                    }
                }
                isVap = true
                file_info["vap_info"] = vapJson
            }
        }
    } else {
        // get sub files
        var subFiles = fs.readdirSync(filePath)
        var absuluteSubFiles = []
        for (var subFileIndex in subFiles) {
            var subFile = subFiles[subFileIndex]
            if (isFileInnerPath(subFile)) {
                continue
            }
            var subFilePath = path.join(filePath, subFile);
            // only keed the mp4 or dir
            if (subFile.endsWith(".mp4") || fs.statSync(subFilePath).isDirectory()) {
                if (subFile.endsWith(".mp4")) {
                    var vapJson = getVapInfo(subFilePath);
                    if (vapJson != null) {
                        absuluteSubFiles.push(subFilePath)
                    } else {
                        console.log("mp4 file is not vap", subFilePath, "skip it");
                    }
                } else {
                    absuluteSubFiles.push(subFilePath)
                }

            }

        }
        result["sub_files"] = absuluteSubFiles
    }
    result["code"] = 0
    result["msg"] = ""
    result["file_info"] = file_info
    result["is_dir"] = isDir
    result["is_vap"] = isVap
    console.log("result:", result)
    res.writeHead(200, { 'Content-Type': 'application/jsonn' })
    res.end(JSON.stringify(result))
}

server_map.set("/file", onFileRequest);

async function onVapFileListRequest(req, params, res) {
    // 这里返回一个文件夹下的所有vap文件
    var filePath = params.get("path")
    console.log("get file info at path:", filePath);
    var fileExist = fs.existsSync(filePath);
    var result = {}
    if (!fileExist) {
        console.log("onFileRequest 1")
        result = {
            "code": -1,
            "msg": "file not exist",
            "file_info": {},
            "is_dir": false,
            "is_vap": false
        }
        res.writeHead(200, { 'Content-Type': 'application/jsonn' })
        res.end(JSON.stringify(result))
        return
    }
    var fileStat = fs.statSync(filePath)
    var isDir = fileStat.isDirectory()
    if (!isDir) {
        console.log("onFlatFileListRequest 1")
        result = {
            "code": -1,
            "msg": "file not directory",
            "file_info": {},
            "is_dir": false,
            "is_vap": false
        }
        res.writeHead(200, { 'Content-Type': 'application/jsonn' })
        res.end(JSON.stringify(result))
        return
    }
    var vapFiles = await get_vap_list_recursive(filePath)
    result["code"] = 0
    result["msg"] = ""
    result["file_list"] = vapFiles
    res.writeHead(200, { 'Content-Type': 'application/jsonn' })
    res.end(JSON.stringify(result))

}



async function get_vap_list_recursive(filePath, vapFiles = []) {
    var stat = fs.statSync(filePath)
    if (!stat.isDirectory()) {
        var file_info = await getFileInfoOfVap(filePath)
        if (file_info != null) {
            vapFiles.push(file_info);
        }
    } else {
        var files = fs.readdirSync(filePath)
        for (var fileIndex in files) {
            var file = files[fileIndex]
            var subFilePath = path.join(filePath, file)
            await get_vap_list_recursive(subFilePath, vapFiles)
        }
    }
    return vapFiles
}

server_map.set("/vap-file-list", onVapFileListRequest);


async function vapInfo(req, params, res) {
    // get vap info avoid complex logic copy up function
    var filePath = params.get("path")
    console.log("get file info at path:", filePath);
    var fileExist = fs.existsSync(filePath);
    var result = {}
    if (!fileExist) {
        console.log("file not exist")
        result = {
            "code": -1,
            "msg": "file not exist",
            "file_info": {},
            "is_dir": false,
            "is_vap": false
        }
        res.writeHead(200, { 'Content-Type': 'application/jsonn' })
        res.end(JSON.stringify(result))
        return
    }
    var fileStat = fs.statSync(filePath)
    var isDir = fileStat.isDirectory()
    var isVap = false
    var file_info = {}
    if (isDir) {
        console.log("file info failed by dir")
        result = {
            "code": -1,
            "msg": "is a dir",
            "file_info": {},
            "is_dir": true,
            "is_vap": false
        }
        res.writeHead(200, { 'Content-Type': 'application/jsonn' })
        res.end(JSON.stringify(result))
        return
    }

    file_info["size"] = fileStat.size;
    result["sub_files"] = []
    // is mp4
    if (!filePath.endsWith(".mp4")) {
        console.log("file info failed by not mp4")
        result = {
            "code": -1,
            "msg": "not a mp4",
            "file_info": {},
            "is_dir": false,
            "is_vap": false
        }
        res.writeHead(200, { 'Content-Type': 'application/jsonn' })
        res.end(JSON.stringify(result))
        return
    }
    var vapJson = getVapInfo(filePath);
    if (vapJson == null) {
        console.log("file info failed by no vap")
        result = {
            "code": -1,
            "msg": "no vap info",
            "file_info": {},
            "is_dir": false,
            "is_vap": false
        }
        res.writeHead(200, { 'Content-Type': 'application/jsonn' })
        res.end(JSON.stringify(result))
        return
    }
    const fileMetaData = await ffprobe(filePath, { path: ffprobeStatic.path })
    if (fileMetaData.streams.length > 0) {
        for (var info in fileMetaData.streams) {
            var stream = fileMetaData.streams[info]
            if (stream.codec_type == "video") {
                file_info["video_info"] = {
                    "codec_name": stream.codec_name,
                    "width": stream.width,
                    "height": stream.height,
                    "duration_ts": stream.duration,
                    "bit_rate": stream.bit_rate,
                }
                break;
            }
        }
        isVap = true
        file_info["vap_info"] = vapJson
    }
    result["code"] = 0
    result["msg"] = ""
    result["file_info"] = file_info
    result["is_dir"] = isDir
    result["is_vap"] = isVap
    console.log("result:", result)
    res.writeHead(200, { 'Content-Type': 'application/jsonn' })
    res.end(JSON.stringify(result))
}

server_map.set("/vap-info", vapInfo);



async function downloadFile(req, params, res) {
    var filePath = params.get("path")
    if (!fs.existsSync(filePath)) {
        // 404
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        res.end('404 Not Found');
        return
    }
    const filename = path.basename(filePath);

    // 设置响应头以告知浏览器这是一个需要下载的文件
    res.setHeader('Content-disposition', 'attachment; filename=' + filename);
    res.setHeader('Content-Type', 'video/mp4');
    // 创建一个读取流并将其管道响应对象
    const readStream = fs.createReadStream(filePath);
    readStream.pipe(res);
}

server_map.set("/download", downloadFile);

async function downloadVapJson(req, params, res) {
    var filePath = params.get("vap-path")
    if (!fs.existsSync(filePath)) {
        // 404
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        res.end('404 Not Found');
        return
    }
    var vapJson = getVapInfo(filePath);
    if (vapJson == null) {
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        res.end('404 Not Found');
        return
    }
    res.writeHead(200, { 'Content-Type': 'application/json' })
    res.end(JSON.stringify(vapJson))

}
server_map.set("/vap-json", downloadVapJson);



function tempVapPathFrom(filePath) {
    var basePath = path.dirname(filePath)
    var basename = path.basename(filePath)
    var tempVapPath = path.join(basePath, "__compress_" + basename)
    return tempVapPath
}

async function getCompressInfo(req, params, res) {
    // get vap compressInfo;
    var filePath = params.get("path")
    if (!fs.existsSync(filePath)) {
        // 404
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        res.end('404 Not Found');
        return
    }

    var compressInfo = {}
    if (compressInfoMap.has(filePath)) {
        compressInfo = compressInfoMap.get(filePath)
    } else {
        var tempVapPath = tempVapPathFrom(filePath)
        if (fs.existsSync(tempVapPath)) {
            var tempVapInfo = getVapInfo(tempVapPath)
            if (tempVapInfo == null) {
                // have temp file but not vap
                compressInfo = {
                    state: CompressState.none,
                    org_path: filePath
                }
            } else {
                var fileInfo = await getFileInfoOfVap(tempVapPath)
                compressInfo = {
                    state: CompressState.done,
                    org_path: filePath,
                    outputPath: tempVapPath,
                    outputFileInfo: fileInfo,
                }
                compressInfoMap[filePath] = compressInfo
            }
        } else {
            compressInfo = {
                state: 0,
            }
        }

    }
    res.writeHead(200, { 'Content-Type': 'application/json' })
    res.end(JSON.stringify(compressInfo))
}
server_map.set("/compress-info", getCompressInfo);



function compressVideo(inputPath, outputPath, speed, quality) {
    console.log("real compress with params spped:", speed, "quality:", quality);
    const ffmpeg = require('fluent-ffmpeg');
    return new Promise((resolve, reject) => {
        const command = ffmpeg(inputPath)
            .outputOptions('-crf', quality) // 设置视频质量，值越小质量越高
            .outputOptions('-preset', speed) // 设置压缩速度，superfast为最快
            .output(outputPath)
            .on('end', resolve)
            .on('error', reject)

        command.on('progress', function (progress) {
            if (compressInfoMap.has(inputPath)) {
                var compressInfo = compressInfoMap.get(inputPath)
                compressInfo.progress = progress.percent
                console.log("压缩进度:", progress.percent)
            }
        });
        command.run();
    });
}



async function startCompress(req, params, res) {
    var filePath = params.get("path")
    var speed = params.get("speed")
    var quality = params.get("quality")
    var needAutoAccept = params.get("auto_accept")
    var tempVapPath = tempVapPathFrom(filePath)
    if (fs.existsSync(tempVapPath)) {
        getCompressInfo(req, params, res)
        return
    }
    var compressInfo = {}
    if (compressInfoMap.has(filePath)) {
        compressInfo = compressInfoMap.get(filePath)
    }
    if (compressInfo.state == CompressState.compressing) {
        res.writeHead(200, { 'Content-Type': 'application/json' })
        res.end(JSON.stringify(compressInfo))
        return
    }
    compressInfo.state = CompressState.compressing;
    compressInfo.needAutoAccept = needAutoAccept === 'true'  // 是否自动保存
    compressInfo.start_time = new Date().getTime()
    compressInfo.org_path = filePath
    compressInfoMap.set(filePath, compressInfo)
    // start compress

    toCompressVideo(filePath, tempVapPath, speed, quality);
    res.writeHead(200, { 'Content-Type': 'application/json' })
    res.end(JSON.stringify(compressInfo))
}

async function toCompressVideo(inputPath, outputPath, speed, quality) {
    compressVideo(inputPath, outputPath, speed, quality)
        .then(() => {
            console.log('Compression finished!')
            if (!compressInfoMap.has(inputPath)) {
                console.log("no compress info");
                // remove output file
                fs.unlinkSync(outputPath)
                return
            }
            console.log("add vapc info")
            var compressInfo = compressInfoMap.get(inputPath)
            compressInfo.outputPath = outputPath
            // 获取到压缩后的文件信息
            getFileInfoOfVap(outputPath, false).then((outputFileInfo) => {
                console.log("这里究竟是什么鬼啊:", outputFileInfo)

                if (outputFileInfo != null) {
                    compressInfo.outputFileInfo = outputFileInfo
                } else {
                    compressInfo.outputFileInfo = "发生了未知错误"
                }
                compressInfo.end_time = new Date().getTime()
                var oldVapInfo = getVapInfo(inputPath)
                if (oldVapInfo != null) {
                    var code, errorMsg = addVapInfoToMp4(outputPath, oldVapInfo)
                    compressInfo.errorCode = code
                    compressInfo.errorMsg = errorMsg
                    // 自动合并
                    if (compressInfo.needAutoAccept != undefined && compressInfo.needAutoAccept == true) {
                        // 判断是否比原始文件小
                        var inputSize = fs.statSync(inputPath).size
                        var outputSize = fs.statSync(outputPath).size
                        if (outputSize > inputSize) {
                            fs.unlinkSync(outputPath)
                            compressInfo.state = CompressState.done
                            compressInfo.progress = 100
                            compressInfo.errorCode = -2
                            compressInfo.errorMsg = "压缩后变大了"
                        } else {
                            var orgVapPath = inputPath
                            if (fs.existsSync(orgVapPath)) {
                                fs.unlinkSync(orgVapPath)
                            }
                            fs.renameSync(outputPath, orgVapPath)
                            compressInfo.state = CompressState.done
                            compressInfo.progress = 100
                        }


                    } else {
                        compressInfo.state = CompressState.done
                        compressInfo.progress = 100
                    }

                } else {
                    compressInfo.state = CompressState.done
                    compressInfo.errorCode = -1
                    compressInfo.errorMsg = "get vap info error"
                }
            }).catch((error) => {
                compressInfo.state = CompressState.done
                compressInfo.errorCode = -1
                compressInfo.errorMsg = error
            })

        })
        .catch(console.error);

}

server_map.set("/start-compress", startCompress);


async function quitCompress(req, params, res) {
    var filePath = params.get("path")
    var tempVapPath = tempVapPathFrom(filePath)
    if (compressInfoMap.has(filePath)) {
        compressInfoMap.delete(filePath)
    }
    if (fs.existsSync(tempVapPath)) {
        // 404
        fs.unlinkSync(tempVapPath)
        res.writeHead(200, { 'Content-Type': 'text/plain' })
        res.end()
        return
    }

    res.writeHead(200, { 'Content-Type': 'text/plain' })
    res.end('')
}

server_map.set("/quit-compress", quitCompress);


async function acceptCompress(req, params, res) {
    var filePath = params.get("path")
    var tempVapPath = tempVapPathFrom(filePath)
    if (!fs.existsSync(tempVapPath)) {
        // 404
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        res.end('404 Not Found');
        return
    }

    var orgVapPath = filePath
    if (fs.existsSync(orgVapPath)) {
        fs.unlinkSync(orgVapPath)
    }
    fs.renameSync(tempVapPath, orgVapPath)
    if (compressInfoMap.has(filePath)) {
        compressInfoMap.delete(filePath)
    }
    res.writeHead(200, { 'Content-Type': 'text/plain' })
    res.end('')
}
server_map.set("/accept-compress", acceptCompress);

async function clear_files(req, params, res) {
    let body = '';
    req.on('data', chunk => {
        console.log('Partial body: ' + body)
        body += chunk.toString(); // 将接收到的数据块转换为字符串并添加到body变量中
    });
    req.on('end', () => {
        const postParams = JSON.parse(body); // 解析POST参数
        var files = postParams["files"]
        for (var fileIndex in files) {
            var filePath = files[fileIndex]
            var tempVapPath = tempVapPathFrom(filePath)
            if (fs.existsSync(tempVapPath)) {
                fs.unlinkSync(tempVapPath)
            }
            if (compressInfoMap.has(filePath)) {
                compressInfoMap.delete(filePath)
            }
        }
        res.end('ok')
    });
}
server_map.set("/clear-files", clear_files);

const server = http.createServer((req, res) => {

    const { url } = req;    // url is somthome like /file?path=xxx
    var urlObj = new URL("http://xxx.com" + url)
    var pathName = urlObj.pathname
    // if pathname in server_map
    res.setHeader('Access-Control-Allow-Origin', '*');
    if (req.method === "OPTIONS") {
        res.setHeader('Access-Control-Allow-Origin', '*');
        res.setHeader('Access-Control-Allow-Methods', 'POST');
        res.setHeader('Access-Control-Allow-Headers', 'content-type');
        res.writeHead(200);
        res.end();
        return;
    }
    if (server_map.has(pathName)) {
        var func = server_map.get(pathName)
        var params = urlObj.searchParams;
        func(req, params, res);
    } else {
        res.setHeader('Access-Control-Allow-Origin', '*');
        res.writeHead(200, { 'Content-Type': 'application/jsonn' })
        res.end(JSON.stringify({}))
    }
})



export function start_file_server() {
    server.listen(3000, '127.0.0.1', () => {
        console.log('Node.js server listening on port 3000')
        var exist = fs.existsSync("/Users/tt/Desktop/build.sh")
        console.log(exist)
    })
}

