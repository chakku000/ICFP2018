# 自動テスト

## 必要

Node 最新版

```
npm i puppeteer
```

## 使い方

```sh
# ふつう
node main.js /path/to/problemsL /path/to/dfltTracesL

# 範囲モード
node main.js /path/to/problemsL /path/to/dfltTracesL 1-100

# 観察モード (50の部分は step per frames)
VIS=50 node main.js /path/to/problemsL /path/to/dfltTracesL
```