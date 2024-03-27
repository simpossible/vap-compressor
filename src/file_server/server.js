const http = require('http')
const fs = require('fs')
const ffprobe = require('ffprobe')
const ffprobeStatic = require('ffprobe-static')
const path = require('path')

import {getVapBoxes, addVapInfoToMp4} from './vap_parser.js'
// md文件访问在electron完全不行。搞个服务来做吧。

// 压缩信息的字典。存储的是路径和压缩信息的关系
var compressInfoMap = new Map()

var server_map = new Map()

function getVapInfo(filePath)  {
    var allBoxes = getVapBoxes(filePath)
    for (var boxIndex in allBoxes) {
        var box = allBoxes[boxIndex]
        if (box.boxType == "vapc") {
            var vapJson = box.content
            return JSON.parse(vapJson)
        }
    }
    return null

}

async function onFileRequest(req, params, res) {

    var filePath = params.get("path")
    console.log("get file info at path:", filePath);
    var fileExist = fs.existsSync(filePath);
    var result = {}
    if (!fileExist) {
        console.log("onFileRequest 1")
        result = {
            "code": -1,
            "msg": "file not exist",
            "file_info":{},
            "is_dir": false,
            "is_vap": false
        }
        res.writeHead(200, { 'Content-Type': 'application/jsonn'}) 
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
    }else {
        // get sub files
        var subFiles = fs.readdirSync(filePath)
        var absuluteSubFiles = []
        for (var subFileIndex in subFiles) {
            var subFile = subFiles[subFileIndex]
            var subFilePath = path.join(filePath, subFile);
            // only keed the mp4 or dir
            if (subFile.endsWith(".mp4") || fs.statSync(subFilePath).isDirectory()) {
                if (subFile.endsWith(".mp4")) {
                    var vapJson = getVapInfo(subFilePath);
                    if (vapJson != null) {
                        absuluteSubFiles.push(subFilePath)
                    }else {
                        console.log("mp4 file is not vap", subFilePath, "skip it");
                    }
                }else {
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
    res.writeHead(200, { 'Content-Type': 'application/jsonn'}) 
    res.end(JSON.stringify(result))
}

server_map.set("/file", onFileRequest);


async function downloadFile(req, params, res){ 
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

async function downloadVapJson(req, params, res){ 
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
    res.writeHead(200, { 'Content-Type': 'application/json'})
    res.end(JSON.stringify(vapJson))

}
server_map.set("/vap-json", downloadVapJson);

async function getCompressInfo(req, params, res){
    // get vap compressInfo; state in [0, 1, 2] 0 == not start, 1 == compressing, 2 == done
    var filePath = params.get("path")
    if (!fs.existsSync(filePath)) {
        // 404
        res.writeHead(404, { 'Content-Type': 'text/plain' }); 
        res.end('404 Not Found');
        return
    }
    var basePath = path.dirname(filePath)
    var basename = path.basename(filePath)
    var tempVapPath = path.join(basePath, ".compress_" + basename + ".mp4")
    if (fs.existsSync(tempVapPath)) {
        // if existed indicated the compress is done
        var compressInfo = {}
        if (compressInfoMap.has(filePath)) {
            compressInfo = compressInfoMap.get(filePath)
            compressInfo.compress_path = tempVapPath
            compressInfo.state = 2
            if (compressInfo.end_time == undefined){
                compressInfo.end_time = new Date().getTime()
            }            
        }else {
            compressInfo.state = 2,
            compressInfo.org_path = filePath,
            compressInfo.compress_path = tempVapPath
            compressInfo.end_time = new Date().getTime()            
            compressInfo.start_time = new Date().getTime()  
            compressInfoMap.set(filePath, compressInfo)
        }
        res.writeHead(200, { 'Content-Type': 'application/json'})
        res.end(JSON.stringify(compressInfo))

        return
    }
    // if not existed
    res.writeHead(200, { 'Content-Type': 'application/json'})
    res.end(JSON.stringify({
        state: 0,
        org_path: filePath
    }))
}
server_map.set("/compress-info", getCompressInfo);



function compressVideo(inputPath, outputPath) {
    const ffmpeg = require('fluent-ffmpeg');
    return new Promise((resolve, reject) => {
        ffmpeg(inputPath)
            .outputOptions('-crf', '23') // 设置视频质量，值越小质量越高
            .outputOptions('-preset', 'fast') // 设置压缩速度，superfast为最快
            .output(outputPath)
            .on('end', resolve)
            .on('error', reject)
            .run();
    });
}



async function startCompress(req, params, res){
    var filePath = params.get("path")
    var basePath = path.dirname(filePath)
    var basename = path.basename(filePath)
    var tempVapPath = path.join(basePath, "__compress_" + basename)
    if (fs.existsSync(tempVapPath)) {
        getCompressInfo(req, params, res)
        return
    }
    var compressInfo = {}
    if (compressInfoMap.has(filePath)) {
        compressInfo = compressInfoMap.get(filePath)     
    }else {
        compressInfo.state = 1,
        compressInfo.start_time = new Date().getTime()
        compressInfo.org_path = filePath        
        compressInfoMap.set(filePath, compressInfo)
    }
    // start compress
    
    compressVideo(filePath, tempVapPath)
    .then(() => {
        console.log('Compression finished!')
        // var oldVapInfo = getVapInfo(filePath)
        // addVapInfoToMp4(tempVapPath, compressInfo.vap_info)
    })
    .catch(console.error);
}

server_map.set("/start-compress", startCompress);




const server = http.createServer((req, res) => {    
       
    const { url } = req;    // url is somthome like /file?path=xxx
    var urlObj = new URL("http://xxx.com" + url)
    var pathName = urlObj.pathname
    // if pathname in server_map
    res.setHeader('Access-Control-Allow-Origin', '*'); 
    if (server_map.has(pathName)) {
        var func = server_map.get(pathName)
        var params = urlObj.searchParams;
        func(req, params, res);
    }else {
        res.setHeader('Access-Control-Allow-Origin', '*'); 
        res.writeHead(200, { 'Content-Type': 'application/jsonn'}) 
        res.end(JSON.stringify({}))
    }   
  })
  


export function start_file_server (){
    server.listen(3000, '127.0.0.1', () => {
        console.log('Node.js server listening on port 3000')
        var exist = fs.existsSync("/Users/tt/Desktop/build.sh")
        console.log(exist)
    })
}

