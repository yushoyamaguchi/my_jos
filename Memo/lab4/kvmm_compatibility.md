# lab4のコードのkvmm対応
初め、kvmmで実行した時だけgeneral protectionの例外が出ていた。
エラーコードは`0x00000173`であった。
https://wiki.osdev.org/Exceptions#General_Protection_Fault
https://wiki.osdev.org/Exceptions#Selector_Error_Code
これを参考にして、エラーコードの意味を調べた。
すると、IDTのselector indexが46(0x2e)であることがわかった。

これは、IRQ_OFFSET + IRQ_IDE_PRIMARYである。
このことから、kvmmからくるideの割り込みが原因であることがわかった。