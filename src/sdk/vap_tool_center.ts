class VapToolCenter {
  constructor() {
    this._tools = {};
  }

  registerTool(name, tool) {
    this._tools[name] = tool;
  }

  getTool(name) {
    return this._tools[name];
  }
}