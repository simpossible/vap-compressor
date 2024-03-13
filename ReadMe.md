这个项目主要是做一个简单的vap的压缩工具
因为vap在mp4的box里面记录了一个名字叫做'vapc'的box
所以我们要压缩vap文件需要
1. 先保存vap-box的数据
2. 使用ffmpeg压缩
3. 再将vap-box的数据追加再mp4的末尾