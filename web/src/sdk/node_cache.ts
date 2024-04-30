
export interface NodeCacheInterface {
  getNodeByPath(path: string): any ;
  cacheNode(node: any, path: string): void;
}

let vapCompresseNodeCache: NodeCacheInterface | null = null;

export function setNodeCache(implement: NodeCacheInterface){
  vapCompresseNodeCache = implement;
}


export function currentNodeCache(): NodeCacheInterface {
  return vapCompresseNodeCache as NodeCacheInterface;
};