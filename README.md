no : `n`eguse's `o`wn language
==============================

## noとは

noは[own language contest](http://olc.humsoft.xyz/)に参加するために[neguse](https://twitter.com/neguse)が作成している自作言語です。
noは以下のような特徴をもちます。

* 問題を解くために必要な分だけの機能を用意
  * それ以外のことはあまり考えない
* シンプルな構文
  * すべてのトークンは1文字で構成される
  * 構文木は二分木で構成される
  * 同じ記号を別の意味で使わない
  * 言語機能はLispっぽく
* シンプルな実装
  * 現時点で700行ぐらいのC++

## noの文法

[design.md](./design.md)を参照ください。

## 使い方

まず、C++11に対応しているバージョンのGCCをインストールしてください。

次に、以下のコマンドで実行ファイルをビルドできます。

```
$ make
```

実行ファイルをビルドしたあと、以下のようにサンプルプログラムを実行できます。

```
$ ./no answer/hello.no
hello world

```

なお、`answer/fibonacci.no`の実行にはかなりの長い時間かかります。
`answer/fibonacci_iter.no`の方は[iterative algorithm](https://mitpress.mit.edu/sicp/chapter1/node13.html)の実装になっていて、こちらは比較的短時間で実行できます。

## プログラムの説明

### hello world

noには文字列リテラルが存在せず、文字列は文字コードのリストとして表現されるため、
各文字の文字コードをconsするプログラムになっています。

noでは数値リテラルは16進数値として書きます。
1桁の数値は`1`のようにそのまま書きますが、2桁の数値は`#68`のように#をつけて書きます。

noの処理系はプログラム中最後の式の評価結果を出力するようになっているため、
プログラム中にprint()のような関数呼び出しはありません。

### square

xと宣言されている関数は1行分の文字列を生成し、yと宣言されている関数はx関数を用いてn行分の文字列を生成します。

`hello world`の説明のとおりnoの処理系はプログラム中最後の式の評価結果を出力しますが、
値がリスト型の場合はリストを『一行ずつの「1文字ずつの文字コードのリスト」のリスト』とみなして出力します。
そのため、x関数はリストを、y関数はリストのリストを生成するようになっています。

### fibonacci number

[SICP](https://mitpress.mit.edu/sicp/chapter1/node13.html)にある通りのiterative algorithmな実装になっています。
noの処理系は現状あまり高速でなく、tree recursiveな実装だとfib(40)の計算にかなりの時間がかかってしまうのですが、
iterative algorithmな実装であれば高速に計算することが可能です。
やっててよかったSICP。

### Editor!

i関数がinsert, r関数がdeleteを行う関数です。
変数名が問題文のものと異なっていますが、これはnoでは16進リテラルとして使われる文字(a~f)が
変数名として利用できないという制限があるためです。つらい…

## TODO

* 5月末までに、own language contestに投稿
* 構文のブラッシュアップ
  * 関数の定義と呼び出しのところ、1引数に限定することでよりシンプルにしたい
* 実装の改良
  * エラーハンドリング
  * バイトコード実装

