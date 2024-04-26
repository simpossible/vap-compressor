import { defineConfig } from 'vite';
import { pluginExposeRenderer } from './vite.base.config.mjs';
import vue from '@vitejs/plugin-vue';
import { nodePolyfills } from 'vite-plugin-node-polyfills';
import optimizer from 'vite-plugin-optimizer';

// https://vitejs.dev/config
export default defineConfig((env) => {
  /** @type {import('vite').ConfigEnv<'renderer'>} */
  const forgeEnv = env;
  const { root, mode, forgeConfigSelf } = forgeEnv;
  const name = forgeConfigSelf.name ?? '';

  /** @type {import('vite').UserConfig} */
  return {
    root,
    mode,
    base: './',
    build: {
      outDir: `.vite/renderer/${name}`,
    },
    plugins: [pluginExposeRenderer(name),
    vue(),
    nodePolyfills(),
    optimizer({
      // 预构建 ipcRenderer 在 Electron 渲染进程中使用
      electron: `const { ipcRenderer } = require('electron'); export { ipcRenderer };`,

      // 这表示 'fs' 与 'node:fs' 同时支持
      // e.g.
      //   `import fs from 'fs'`
      //   or
      //   `import fs from 'node:fs'`
      fs: () => ({
        // 这与 `alias` 行为一致
        find: /^(node:)?fs$/,
        code: `const fs = require('fs'); export { fs as default }`
      }),
    })],
    resolve: {
      preserveSymlinks: true,
    },
    clearScreen: false,
  };
});
