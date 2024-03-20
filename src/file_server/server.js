const http = require('http')
const fs = require('fs')

// md文件访问在electron完全不行。搞个服务来做吧。


var server_map = new Map()

server_map.set("/file", function(req, params){
    var filePath = params["path"]
    var fileExist = fs.existsSync(filePath);
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
    if (isDir) {
        file_info["size"] = fileStat.size;
    }
    return {
        "code": 0,
        "msg": "",
        "file_info": file_info,
        "is_dir": isDir
    }
});

const server = http.createServer((req, res) => {    
    res.writeHead(200, { 'Content-Type': 'application/jsonn'})

    const { url } = req;    // url is somthome like /file?path=xxx
    var urlObj = new URL("http://xxx.com" + url)
    var pathName = urlObj.pathname
    // if pathname in server_map
    if (server_map.has(pathName)) {
        var func = server_map.get(pathName)
        var params = urlObj.searchParams;
        
        var result = func(req, params);
        res.end(JSON.stringify(result))
    }else {
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

