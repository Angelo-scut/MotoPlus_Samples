readMe.txt

1. Abstraction

This sample program is as same as the TCP server program written in [New Language Environment
 MOTO-PLUS Programmers Manual ].
FS100 side TCP server translates received strings which are sent by Windows PC client to large
 character and send back (Echo back) to the PC. 

Windows PC client application program is on a folder 
 C:\MotoPlusData\sample\Ethernet control\TCP\clientForWindows\client.exe

2. How to use
1) For preparation, connect between FS100 and the PC via network cable and configure IP address 
   and so on in advance. 
2) Install sample server application to FS100 
3) startup FS100
4) Start PC side client application "client.exe" via DOS prompt window. 
5) After stating client application, this application requires to set server port number and IP address.
   You should set port number = 11000, IP address = FS100 IP address.
6) Insert character via key board and enter them, the same strings but large one are displayed on the screen.
7) Insert "exit<CR>" the client is terminated. 

	
  ƒExecution example of DOS prompt„

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
