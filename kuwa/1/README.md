# ri-domi-

## Preparation

```
make # all
```

## モデルを可視化する(見づらい)

```
./model ./path/to/target.mdl
```

## 命令列を普通の文字列で出力

合ってるといいなぁ

```
./decode_trace ./path/to/trace.nbt
```

## 読める命令列を既定のバイナリで出力

```
./encode_trace ./path/to/trace.txt
```


## 命令列を実行して、targetと一致するか調べる

```
./interpret ./path/to/target.mdl ./path/to/trace.nbt
```

## デフォルト実装の `SMove` を圧縮する

```
./compress ./path/to/trace.nbt
```

## デフォルト実装のharmonicsをいい感じにする

```
./opt1 ./path/to/target.mdl ./path/to/trace.nbt
```
