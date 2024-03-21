const fs = require('fs')

class Mp4Box {    
    constructor(boxType, start, size, content) {
        this.boxType = boxType
        this.start = start
        this.size = size
        this.content = content
    }

}


export function get_vap_boxes(file_path) {
    console.log("start parse mp4");
    var fd = fs.openSync(file_path, 'r')
    var allBoxes = new Array()
    var position = 0
    while (true) {
        var boxStart = position;
        var sizeReadBuffer = Buffer.alloc(4)
        var sizeRead = fs.readSync(fd, sizeReadBuffer, {
            position: position,
            length: 4
        })
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
        if (boxTypeRead != 4) {
            console.log("read file error")
            break;
        }
        position += 4
        var boxType = boxTypeReadBuffer.toString()
        console.log("boxType:", boxType)


        if (boxSize == 1){
            var largeSizeReadBuffer = Buffer.alloc(8)
            var largeSizeRead = fs.readSync(fd, largeSizeReadBuffer, {
                position: position,
                length: 8
            })
            if (largeSizeRead != 8) {
                console.log("read file error")
                break;
            }
            position += 8
            boxSize = largeSizeReadBuffer.readBigUInt64BE()
        }
        var boxContent = ""
        if (boxType == "vapc") {
            var contentReadBuffer = Buffer.alloc(boxSize)
            var contentRead = fs.readSync(fd, contentReadBuffer, {
                position: position,
                length: boxSize
            })
            position += boxSize;
            if (contentRead != boxSize) {
                console.log("read file error")
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