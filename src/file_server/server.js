const http = require('http')
const fs = require('fs')

import {get_vap_boxes} from './vap_parser.js'
// md文件访问在electron完全不行。搞个服务来做吧。


var server_map = new Map()

server_map.set("/file", function(req, params){

    var filePath = params.get("path")
    console.log("get file info at path:", filePath);
    var fileExist = fs.existsSync(filePath);
    var result = {}
    if (!fileExist) {
        return {
            "code": -1,
            "msg": "file not exist",
            "file_info":{},
            "is_dir": false
        }
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
            var all_boxes = get_vap_boxes(filePath)

        }
    }else {
        // get sub files
        var subFiles = fs.readdirSync(filePath)
        result["sub_files"] = subFiles
    }
    result["code"] = 0
    result["msg"] = ""
    result["file_info"] = file_info
    result["is_dir"] = isDir
    return result
});

const server = http.createServer((req, res) => {    
       
    const { url } = req;    // url is somthome like /file?path=xxx
    var urlObj = new URL("http://xxx.com" + url)
    var pathName = urlObj.pathname
    // if pathname in server_map
    if (server_map.has(pathName)) {
        var func = server_map.get(pathName)
        var params = urlObj.searchParams;
        
        var result = func(req, params);
        res.setHeader('Access-Control-Allow-Origin', '*'); 
        res.writeHead(200, { 'Content-Type': 'application/jsonn'}) 
        res.end(JSON.stringify(result))
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

