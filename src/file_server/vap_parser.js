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