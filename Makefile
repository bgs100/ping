client:
	g++ main.cpp -Wall -lSDL2 -lSDL2_ttf -o ping

server: Server.cpp
	g++ Server.cpp -Wall -lSDL2_net -o server
