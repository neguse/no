## 問題を解くために必要な機能

* 標準入力から読み込む
  * 要素はスペースで区切られている
* 標準出力への出力
* リテラルの作成
* add, sub
* 関数定義
* 関数呼び出し
  * 再帰
* 文字列の挿入、削除

## 基本ポリシー

* シンプルな実装にする
  * 問題の解答についても、言語処理系についても

## 言語仕様

* 演算子、変数、組み込み関数などは1文字で表現する
* 1つの文字は常に1つの意味のみを持たせる
* 構文木は二分木とする
* プログラムは式のリストとする
* プログラムの最後の式の評価結果を出力する

## 型

変数は型をもたない

* integer
* list
  * 文字列はintegerのlistとして表現される
* closure

## トークン

```

! (親) : 代入

~ (親) : 関数呼び出し
\ (親) : 関数定義(lambda)
_ (子) : 関数呼び出しの際に引数がないことを示す
( (親) : 関数呼び出しの引数リストの開始
, (子) : 関数呼び出しの引数リストの区切り
) (子) : 関数呼び出しの引数リストの終了
[ (親) : 関数定義の引数リストの開始
. (子) : 関数定義の引数リストの区切り
] (子) : 関数定義の引数リストの終了

+ (親) : add
- (親) : sub
= (親) : equal
< (親) : less than
> (親) : greater than

? (親) : if
| (親) : 1つ目の子がthen節、2つ目の子がelse節

# (親) : 2桁の16進数値

: (親) : cons

```

## 組み込み関数、変数

組み込み関数は`~`で呼び出す

```
I : read
A : car
D : cdr
N : nil
```

## 構文

```
program    = "{" { expression ";" } "}" ;
expression = assign_exp | call_exp | lambda_exp | op_exp | if_exp | hex2 | hex | variable ;
assign_exp = "!" variable expression ;
call_exp   = "~" expression arg_list ;
call_arg_list   = "(" { expression "," } ")" ;
lambda_exp = "\" lambda_arg_list expression ;
lambda_arg_list   = "[" { variable "." } "]" ;
op_exp     = op expression expression ;
op         = "+" | "-" | "<" | ">" | ":" ;
if_exp     = "?" expression "|" expression expression ;
hex2       = # hex hex ;
hex        = 数値, a〜fまでのアルファベット
variable   = g〜z, A〜Zまでのアルファベット
```

