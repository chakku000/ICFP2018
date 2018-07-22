使い方
====

```sh
# 常にHighモードで実行する
# 各ケースを実行して、バイナリファイルを出力
sh generate.sh

# 常にLowモードで実行する
# groundedチェックが走るようにしてあるので重い
# (groundedをチェックしないなら`multi-low-line.py`内の`state.check_grounded(True)`をコメントアウトする)
sh low-generate.sh
```

指定できるパラメタは用意してないので、`generate.sh`を書き換えてうまく使う
