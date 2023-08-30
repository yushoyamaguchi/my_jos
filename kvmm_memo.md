# kvmm対応における変更点
- fs.imgをつくるためのスクリプトをMakefileに追加
- kvmmはメモリの空き領域読み取りをハードウェアエミュレートしてないので、pmap.cのdetect_memory()を修正