const fs = require('fs');

function isMP4File(filePath) {
    // 检查文件是否存在
    if (!fs.existsSync(filePath)) {
        return false;
    }

    // 检查文件扩展名是否为 .mp4
    const ext = path.extname(filePath);
    return ext.toLowerCase() === '.mp4';
}