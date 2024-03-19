import fs from 'fs';


class VapFileGroup implements VapResource {
    src: string;
    files: VapFile[] = [];
    constructor() {
        this.src = ""; // path to the directory        
    }
  initial(): void {
    if (!fs.lstatSync(this.src).isDirectory()) {
      return;
    }
  }


}