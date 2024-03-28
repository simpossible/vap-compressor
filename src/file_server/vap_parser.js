const fs = require('fs')

class Mp4Box {    
    constructor(boxType, start, size, content) {
        this.boxType = boxType
        this.start = start
        this.size = size
        this.content = content
    }

}


export function getVapBoxes(file_path) {
    console.log("start parse mp4");
    var fd = fs.openSync(file_path, 'r')
    var allBoxes = new Array()
    var position = 0
    while (true) {
        var boxStart = position;
        var sizeReadBuffer = Buffer.alloc(4)
        var headerSize = 0
        var sizeRead = fs.readSync(fd, sizeReadBuffer, {
            position: position,
            length: 4
        })
        headerSize = headerSize + 4
        if (sizeRead != 4) {
            console.log("read file end or error")
            break;
        }
        position += 4
        var boxSize = sizeReadBuffer.readUInt32BE()
        console.log("boxSize:", boxSize)

        var boxTypeReadBuffer = Buffer.alloc(4)
        var boxTypeRead = fs.readSync(fd, boxTypeReadBuffer, {
            position: position,
            length: 4
        })
        headerSize = headerSize + 4
        if (boxTypeRead != 4) {
            console.log("read file error - 1")
            break;
        }
        position += 4
        var boxType = boxTypeReadBuffer.toString()
        
        if (boxSize == 1){
            var largeSizeReadBuffer = Buffer.alloc(8)
            var largeSizeRead = fs.readSync(fd, largeSizeReadBuffer, {
                position: position,
                length: 8
            })
            if (largeSizeRead != 8) {
                console.log("read file error - 2")
                break;
            }
            position += 8
            boxSize = largeSizeReadBuffer.readBigUInt64BE()
            headerSize = headerSize + 8
        }
        var boxContent = ""
        if (boxType == "vapc") {
            console.log('box size is ', boxSize, "headersize is ", headerSize, "position is ", position)
            var vapcLen = boxSize - headerSize
            var contentReadBuffer = Buffer.alloc(vapcLen)
            var contentRead = fs.readSync(fd, contentReadBuffer, {
                position: position,
                length: vapcLen
            })
            console.log("vaplen is ", vapcLen, "content read is ", contentRead);
            position += vapcLen;
            if (contentRead != vapcLen) {
                console.log("read file error - 3")
                break;
            }
            boxContent = contentReadBuffer.toString()            
        }
        var box = new Mp4Box(boxType, boxStart, boxSize, boxContent)        
        allBoxes.push(box)        
        position = boxStart + boxSize
    }    
    return allBoxes    
}

export function addVapInfoToMp4(filePath, vapJson) {
    var allBoxes = getVapBoxes(filePath)
    for (var boxIndex in allBoxes) {
        var box = allBoxes[boxIndex]
        if (box.boxType == "vapc") {
            return -1, "already vap"
        }
    }
    // add a box ftype=vapc data=vapJson to the mp4 file
    var vapJsonStr = JSON.stringify(vapJson)
    var vapJsonBuffer = Buffer.from(vapJsonStr)
    var vapJsonBufferLen = vapJsonBuffer.length
    var vapcBoxSize = 8 + vapJsonBufferLen
    var vapcBoxBuffer = Buffer.alloc(vapcBoxSize)
    vapcBoxBuffer.write("vapc", 0)
    vapcBoxBuffer.writeUInt32BE(vapcBoxSize, 4)
    vapJsonBuffer.copy(vapcBoxBuffer, 8)

    var fileBaseName = path.basename(filePath)
    var fileDir = path.dirname(filePath)
    var tempFilePath = path.join(fileDir, "__temp" + fileBaseName)
    var fd = fs.openSync(tempFilePath, 'w')    
    var oldFd = fs.openSync(filePath, 'r')
    var position = 0
    for (var oldBox of allBoxes) {
        var oldBoxBuffer = Buffer.alloc(oldBox.size)
        fs.readSync(oldFd, oldBoxBuffer, {
            position: oldBox.start,
            length: oldBox.size
        })
        fs.writeSync(fd, oldBoxBuffer, {
            position: oldBox.start
        })
        position = oldBox.start + oldBox.size
    }
    fs.writeSync(fd, vapcBoxBuffer, {
        position: position
    })


    return 0, ""
}