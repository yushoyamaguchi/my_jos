# Makefileにおける依存関係
```
AAA: BBB CCC
    XXX
```
みたいになってるとき、AAAはBBBに依存している。
BBBが変更されたとき、AAA(つまりXXX)が実行される。
