# 1009時点でのエラー
## 
ld: warning: cannot find entry symbol _start; defaulting to 0000000000800020


# 
libc_support.cみたいなのを作って、その中でwriteとかのラッパーを用意して、そのオブジェクトファイルとリンクする必要がある？