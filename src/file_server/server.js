const http = require('http')
const fs = require('fs')
const ffprobe = require('ffprobe')
const ffprobeStatic = require('ffprobe-static')
const path = require('path')

import {getVapBoxes} from './vap_parser.js'
// md文件访问在electron完全不行。搞个服务来做吧。


var server_map = new Map()

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
            "is_dir": false
        }
        res.writeHead(200, { 'Content-Type': 'application/jsonn'}) 
        res.end(JSON.stringify(result))
        return
    }
    var fileStat = fs.statSync(filePath)
    var isDir = fileStat.isDirectory()
    var file_info = {}
    if (!isDir) {
        file_info["size"] = fileStat.size;        
        result["sub_files"] = []
        // is mp4
        if (filePath.endsWith(".mp4")) {
            file_info["is_mp4"] = true
            // get mp4 size resolution
            var allBoxes = getVapBoxes(filePath)
            for (var boxIndex in allBoxes) {
                var box = allBoxes[boxIndex]
                if (box.boxType == "vapc") {
                    var vapJson = box.content
                    file_info["vap_json"] = JSON.parse(vapJson)
                }
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
            }
            console.log("fileMetaData:", fileMetaData)

        }
    }else {
        // get sub files
        var subFiles = fs.readdirSync(filePath)
        var absuluteSubFiles = []
        for (var subFileIndex in subFiles) {
            var subFile = subFiles[subFileIndex]
            var subFilePath = path.join(filePath, subFile);
            absuluteSubFiles.push(subFilePath)
        }
        result["sub_files"] = absuluteSubFiles
    }
    result["code"] = 0
    result["msg"] = ""
    result["file_info"] = file_info
    result["is_dir"] = isDir
    console.log("result:", result)
    res.writeHead(200, { 'Content-Type': 'application/jsonn'}) 
    res.end(JSON.stringify(result))
}

server_map.set("/file", onFileRequest);

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

