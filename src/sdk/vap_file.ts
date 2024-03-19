import fs from 'fs';

class VapFile implements VapResource {
    src: string
    file_size: number
    video_width: number
    video_height: number
    video_duration: number
    video_bitrate: number
    constructor() {
      this.src = "" // Path to the file
      this.file_size = 0
      this.video_width = 0
      this.video_height = 0
      this.video_duration = 0
      this.video_bitrate = 0      
    }

    initial(): void {
      // get the file info
      if (!fs.lstatSync(this.src).isFile()) {
        return
      }
      const file_info = ffmpeg.ffprobeSync(this.src)
      this.file_size = file_info.format.size
      this.video_width = file_info.streams[0].width
      this.video_height = file_info.streams[0].height
      this.video_duration = file_info.format.duration
      this.video_bitrate = file_info.format.bit_rate
    }

    get_vapc_box(): string {
      // src is a mp4 file, the vap file should have a box named 'vapc'
      
    }
  }