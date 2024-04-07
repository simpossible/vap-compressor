
export var UrlPathCompressInfo = 'compress-info';
export var UrlPathFile = 'file';
export var UrlPathStartCompress = 'start-compress';
export var UrlPathQuitCompress = 'quit-compress';
export var UrlPathAcceptCompress = 'accept-compress';
export var UrlPathVapInfo = 'vap-info';
export var UrlPathDownload = 'download';
export var UrlPathVapJson = 'vap-json';
export var UrlPathVapList = 'vap-file-list';

var vapServerPort = 3000;

export function vapUrlForKey(urlKey, params) {
  // params is map of key-value pairs
  var tempUrl = 'http://127.0.0.1:' + vapServerPort + '/' + urlKey;
  if (params != undefined && params != null) {
    var first = true;
    for (var key in params) {
      if (first) {
        tempUrl += '?';
        first = false;
      } else {
        tempUrl += '&';
      }
      tempUrl += key + '=' + params[key];
    }
  }
  return tempUrl;
}



