all: client server

client:
	g++ GameManager.cpp Game.cpp ButtonMenu.cpp TitleScreen.cpp DifficultyMenu.cpp MultiplayerMenu.cpp ErrorScreen.cpp KeyboardInput.cpp AIInput.cpp Texture.cpp utility.cpp -Wall -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_net -o ping

server:
	g++ Server.cpp -Wall -lSDL2_net -o server
