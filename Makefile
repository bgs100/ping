all: client server

client: Game.cpp GameManager.cpp utility.cpp
	g++ GameManager.cpp Game.cpp utility.cpp -Wall -lSDL2 -lSDL2_ttf -lSDL2_net -o ping

server: Server.cpp
	g++ Server.cpp -Wall -lSDL2_net -o server
