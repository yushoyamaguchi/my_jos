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
<br>
nextfreeの先頭アドレスはbss領域の末端になっている


# page_init
基本的にはメモリを未使用のフラグを立てるのだが、例外もあるのでそれを設定する
memlayout.hに、例外となるアドレスの定義がされている

ここで、pagesが指す物理アドレスが使って良い領域であるかのチェックもしておく。
それによって、今後pagesのアドレスを変換するだけで使って良い仮想アドレスが得られるようになる


# page_alloc
末尾から使って、free_listが尽きたら終わり
pp_link(1個前のfree状態のpageへのポインタ)をきちんと更新する
page_free_listは使う
ここではメモリ管理リストへの処理だけ行って、実際にallocした領域の仮想addrの提供(とページング関連の処理は)あとでやる？
<br>
何かエラーが起こったら、ここをチェックしに来たほうがよさそう

# page_free
freeした領域を担当するPageInfo構造体の、メモリ管理リストへの追加をする

# kern_pgdir
kern_pgdirははじめ何もマッピングされていないため、pdgir_XXXみたいな関数を使って、必要なときに自分でマッピングをする必要がある。
pgtableはまとめて構築するのではなく、必要になったときにpte_t型の領域をpgdirに登録する。
この登録はboot_map_region()を使って行う。

# boot_map_region
boot_map_region()は、pgdirに対して、仮想アドレスvaから始まる領域を、物理アドレスpaから始まる領域にマッピングする。

pgdir_walkを使う

# pgdir_walk
pgdir_walk()は、pgdirに対して、仮想アドレスvaに対応するページテーブルのエントリを返す。

- この関数が呼び出された際、新たに対応付けを行うたびに、新しいpgtableを確保する。
(なので、はじめはそのpgtableの1024個のうち入ってるエントリは1個だけ)
- この関数が呼び出された際、該当するpgtableがすでに確保されている場合は、そのpgtableのエントリを返す。
- pgtableのサイズと1ページのサイズは同じなので、pgtableを確保するときは、page_alloc()を使うだけでよい


## 備考
MMUに登録されているpgdir,pgtableから割り出せる仮想アドレス・物理アドレスの関係と、page2paで対応付けられるpageinfo配列の要素・物理アドレスの関係は、別物っぽい。
(pageinfo配列はあくまでも物理アドレスの使用状況把握のためのものだから？)