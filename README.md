# project-alpha-duino

リーダーサーバーと通信する物理部分です。

### 機能
- 鍵の有無を検出
- 鍵を 持ってきた・返却した・一時退出 を指定する物理ボタンのコントロール
- 想定外の操作をされたときブザーで警告する
- 上の操作をリーダーサーバーに転送する


### ハードウェア
- Arduino Nano
- 上を搭載した自作基板


### 言語
- Arduino


### 通信
- Arduino Nano に内蔵されている USB-Serial を使用し、リーダーサーバーの Raspberry Pi と通信する。
- 全てのシリアル通信はバイナリで行われる。


### 主な動作
#### 起動直後
コントローラーは起動後待ち受け状態になり、リーダーサーバーからの要求に対して現在の状態を返答する。コントローラーは自発的にデータを送信しない。  
起動後五秒間は全てのボタンが点滅し、全てのボタンは反応しない。

#### データの取得
1. リーダーサーバーが任意の1バイトデータを生成し、コントローラーに送信する。
2. コントローラーは任意のデータを受信すると、受信してから約30ms以内に3バイトの返答を行う。データフォーマットは後述。

#### 操作無く鍵を移動されたとき
(未実装)  
持ち込み・返却 ボタンなどを押さずに鍵を移動させたとき、ブザーで正しく操作をするように警告する。


### データ構造
#### データ要求
1バイトの任意データ。

#### レスポンス
ヘッダを含め3バイトです。

| ヘッダ | データ | チェック用データ |
| :--- | :--- | :--- |
| 0x00 | 1バイト | 1バイト |

ヘッダ：データの同期を取るために挿入する。0x00固定。  
データ：各ビットがボタン・鍵状態を表す。構造は以下の通りです。。  

| BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :---|
| 固定 | 予約 | 予約 | 予約 | ボタン3 | ボタン2 | ボタン1 | 鍵状態 |
| 1 | 0 | 0 | 0 | 押されると 1 | 同左 | 同左 | 鍵が存在するとき 1 |

チェック用データ：データを反転させた値が入ります。  
データが 0b10000011 の場合、チェック用データは 0b01111100 となります。