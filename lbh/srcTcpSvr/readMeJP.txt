readMe.txt

１．概要

	このサンプルは、「MotoPlusプログラマーズマニュアル」に掲載のTCPサーバ
	サンプルと同等のものです。
	FS100側のTCPサーバは、WindowsPCのクライアントが送信した文字列を大文字
	に変換してエコーバックします。

	WindowsPC用のクライアントアプリケーションがこのディレクトリ配下にあり
	ます。
	
	① C:\MotoPlusData\sample\Ethernet control\TCP\clientForWindows\client.exe

２．使い方
	
	予めFS100とパソコンのネットワークケーブルを接続して、IPアドレスなどの
	設定を行っておきます。
	その後、サンプルサーバーアプリをFS100に導入しFS100を起動してから、
	クライアントをDOSプロンプトで起動して下さい。

	(1) サーバアプリをFS100にロードします
	(2) FS100を再起動します
	(3) クライアントアプリ(①)をDOSプロンプトで起動します
	(4) クライアントアプリを起動すると、サーバのポート番号とIPアドレスの
	　　入力を求めてきます。ポート番号は：11000を、IPアドレスは予め設定した
	　　通り入力してください
	(5) 引き続き、キーボードから文字を入力しエンタキーを押下する度に、
	　　サーバーへの送信と受信したデータを表示します
	(6) exit<CR> と入力すると、クライアントを終了します
	
  ＜DOSプロンプト実行例＞

	C:\MotoPlusData\sample\Ethernet control\TCP\clientForWindows>client -----------<1>
	Simple TCP client
	Please type PORT No. >11000
	Please type IP addr. >10.0.0.2
	Please type the ASCII characters
	> a
	send(1) a
	recv(2) A -----------------------------------------<2>
	> b
	send(3) b
	recv(4) B
	> exit
	send(5) exit
	recv(6) EXIT

	C:\MotoPlusData\sample\Ethernet control\TCP\clientForWindows> -----------------<3>


以上