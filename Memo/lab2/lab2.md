# メモ
qemuモニターのメモリダンプは物理アドレス
gdbのメモリダンプは、仮想アドレス？


# 改造すべき箇所
boot_alloc() fin
mem_init() (only up to the call to check_page_free_list(1))
page_init()
page_alloc()
page_free() 


# 全体
メモリ管理には、物理ページの管理と仮想アドレスの管理の2つがある

# 物理ページの管理
PageInfo構造体の配列と物理ページの対応関係を辿れるようなデータ構造にしておく
<br>
page_tableに入れるのは物理メモリ
物理メモリの0~0x3fffffまでが、はじめにentry_pgdirの(KERNBASE>>PDXSHIFT)番目のpage_tableにて、マッピングされている。


# mem_init
Pageinfo構造体の配列のallocする
(nextfreeの先頭アドレスはbss領域の末端になっている)

# page_init
基本的にはメモリを未使用のフラグを立てるのだが、例外もあるのでそれを設定する
memlayout.hに、例外となるアドレスの定義がされている


# page_alloc
末尾から使って、free_listが尽きたら終わり
pp_link(1個前のfree状態のpageへのポインタ)をきちんと更新する
page_free_listは使う
ここではメモリ管理リストへの処理だけ行って、実際にallocした領域の仮想addrの提供(とページング関連の処理は)あとでやる？

# page_free
freeした領域を担当するPageInfo構造体の、メモリ管理リストへの追加をする

# kern_pgdir
kern_pgdirははじめ何もマッピングされていないため、pdgir_XXXみたいな関数を使って、必要なときに自分でマッピングをする必要がある。
pgtableはまとめて構築するのではなく、必要になったときにpte_t型の領域をpgdirに登録する。


## 備考
MMUに登録されているpgdir,pgtableから割り出せる仮想アドレス・物理アドレスの関係と、page2paで対応付けられるpageinfo配列の要素・物理アドレスの関係は、別物っぽい。
(pageinfo配列はあくまでも物理アドレスの使用状況把握のためのものだから？)