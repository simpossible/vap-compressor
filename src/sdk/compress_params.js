export var CompressSpeedOptions = [
  'veryslow',
  'slower',
  'slow',
  'medium',
  'fast',
  'faster',
  'veryfast',
  'superfast',
  'ultrafast'
]

var __chineseMap = new Map([
  [1, '一'],
  [2, '二'],
  [3, '三'],
  [4, '四'],
  [5, '五'],
  [6, '六'],
  [7, '七'],
  [8, '八'],
  [9, '九']
]);

export function compressSpeedOptionDisplayName(optName) {
  var index = CompressSpeedOptions.indexOf(optName)
  var chinesIndex = __chineseMap.get(index + 1)
  return chinesIndex + "挡"    
}