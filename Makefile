all: client server

client: GameManager.cpp Game.cpp TitleScreen.cpp Texture.cpp utility.cpp
	g++ GameManager.cpp Game.cpp TitleScreen.cpp MultiplayerMenu.cpp Texture.cpp utility.cpp -Wall -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_net -o ping

server: Server.cpp
	g++ Server.cpp -Wall -lSDL2_net -o server
