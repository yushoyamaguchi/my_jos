# メモ
qemuモニターのメモリダンプは物理アドレス
gdbのメモリダンプは、仮想アドレス？


# 改造すべき箇所
boot_alloc() fin
mem_init() (only up to the call to check_page_free_list(1))
page_init()
page_alloc()
page_free() 

pgdir_walk()
boot_map_region()
page_lookup()
page_remove()
page_insert()


# 全体
メモリ管理には、物理ページの管理と仮想アドレスの管理の2つがある

# 物理ページの管理
PageInfo構造体の配列と物理ページの対応関係を辿れるようなデータ構造にしておく
<br>
page_tableに入れるのは物理メモリ
物理メモリの0~0x3fffffまでが、はじめにentry_pgdirの(KERNBASE>>PDXSHIFT)番目のpage_tableにて、マッピングされている。

# boot_alloc
pgdir差し替え前も差し替え後も、kalloc的な使い方ができる

# mem_init
Pageinfo構造体の配列のallocする
<br>
nextfreeの先頭アドレスはbss領域の末端になっている
<br>
使う物理アドレス空間の全領域を仮想アドレス空間に割り当てる
<br>
よって以後、pa2vaみたいなことが足し算引き算だけでできるようになる
(page_insert関数で、ここでやったのとは異なる対応付けがされてる領域には注意)

# page_init
基本的にはメモリを未使用のフラグを立てるのだが、例外もあるのでそれを設定する
memlayout.hに、例外となるアドレスの定義がされている

ここで、pagesが指す物理アドレスが使って良い領域であるかのチェックもしておく。
それによって、今後pagesのアドレスを変換するだけで使って良い物理アドレス(ひいては仮想アドレスも)が得られるようになる


# page_alloc
末尾から使って、free_listが尽きたら終わり
pp_link(1個前のfree状態のpageへのポインタ)をきちんと更新する
page_free_listは使う
ここではメモリ管理リストへの処理だけ行って、実際にallocした領域の仮想addrの提供(とページング関連の処理は)あとでやる？
<br>
何かエラーが起こったら、ここをチェックしに来たほうがよさそう
<br>
freeもそうだが、この関数に関しては、pageinfo配列の管理だけを行っている
(実態を配列に対応させるのは別の関数の役目)

# page_free
freeした領域を担当するPageInfo構造体の、メモリ管理リストへの追加をする

# kern_pgdir
kern_pgdirははじめ何もマッピングされていないため、pdgir_XXXみたいな関数を使って、必要なときに自分でマッピングをする必要がある。
pgtableはまとめて構築するのではなく、必要になったときにpte_t型の領域をpgdirに登録する。
この登録はboot_map_region()を使って行う。

# boot_map_region
boot_map_region()は、pgdirに対して、仮想アドレスvaから始まる領域を、物理アドレスpaから始まる領域にマッピングする。

pgdir_walkを使う

名前の通り、boot_map_region()は、paging設定完了前の段階でのみ呼び出される。
(以降はpage_insertを使う？)
# pgdir_walk
pgdir_walk()は、pgdirに対して、仮想アドレスvaに対応するページテーブルのエントリを返す。

- この関数が呼び出された際、新たに対応付けを行うたびに、新しいpgtableを確保する。
(なので、はじめはそのpgtableの1024個のうちpgtable_entryが入っているのは1個だけ)
- この関数が呼び出された際、該当するpgtableがすでに確保されている場合は、そのpgtableのエントリを返す。
- pgtableのサイズと1ページのサイズは同じなので、pgtableを確保するときは、page_alloc()を使うだけでよい

# page_insert
page_insert()は、物理ページppを、仮想アドレスvaに対応付ける。
これを呼び出せるのは、ページングの設定が終わった後。
それまでは、boot_map_region()を使う。


とは言っても、どっちもpgdir_walkに多くの処理を任せているが



## 備考
MMUに登録されているpgdir,pgtableから割り出せる仮想アドレス・物理アドレスの関係と、page2paで対応付けられるpageinfo配列の要素・物理アドレスの関係は、別物っぽい。
(pageinfo配列はあくまでも物理アドレスの使用状況把握のためのものだから？)