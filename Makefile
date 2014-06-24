CXX=g++
CXXFLAGS=-Wall
CPPFLAGS=-MD -MP
LDFLAGS=-Wall
PING_LIBS=-lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_net
PING_SRCS=GameManager.cpp Game.cpp SharedState.cpp ButtonMenu.cpp TitleScreen.cpp DifficultyMenu.cpp MultiplayerMenu.cpp DevConsole.cpp ErrorScreen.cpp KeyboardInput.cpp AIInput.cpp Texture.cpp Socket.cpp utility.cpp
PING_OBJS=$(PING_SRCS:.cpp=.o)
SERVER_LIBS=-lSDL2 -lSDL2_net
SERVER_SRCS=Server.cpp SharedState.cpp utility.cpp
SERVER_OBJS=$(SERVER_SRCS:.cpp=.o)
SRCS=$(PING_SRCS) $(SERVER_SRCS)

all: ping server

ping: $(PING_OBJS)
	$(CXX) $(PING_OBJS) $(LDFLAGS) $(PING_LIBS) -o ping

server: $(SERVER_OBJS)
	$(CXX) $(SERVER_OBJS) $(LDFLAGS) $(SERVER_LIBS) -o server

clean:
	rm *.o *.d

-include $(SRCS:.cpp=.d)
