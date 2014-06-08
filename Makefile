all: client server

client: Game.cpp
	g++ Game.cpp -Wall -lSDL2 -lSDL2_ttf -lSDL2_net -o ping

server: Server.cpp
	g++ Server.cpp -Wall -lSDL2_net -o server
