
CC = g++

LDFLAGS = -lsodium -lssl -lcrypto

CFLAGS = -Wall -Werror -Wextra -g3 -Wno-deprecated-declarations

SRC1 = main.cpp SocketBase.cpp SocketServer.cpp Utils.cpp
EXEC1 = srv
OBJDIR1 = server/obj/
SRCDIR1 = server/src/
SRCS1 = $(addprefix $(SRCDIR1), $(SRC1))
OBJ1 = $(SRCS1:$(SRCDIR1)%.cpp=$(OBJDIR1)%.o)

SRC2 = main.cpp SocketClient.cpp User.cpp
EXEC2 = clt
OBJDIR2 = client/obj/
SRCDIR2 = client/src/
SRCS2 = $(addprefix $(SRCDIR2), $(SRC2))
OBJ2 = $(SRCS2:$(SRCDIR2)%.cpp=$(OBJDIR2)%.o)

all : $(EXEC1) $(EXEC2)

$(EXEC1) : $(OBJ1) $(OBJ3)
	$(CC) -o $@ $^ $(LDFLAGS)

$(EXEC2) : $(OBJ2) $(OBJ3)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR1)%.o : $(SRCDIR1)%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR2)%.o : $(SRCDIR2)%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -rf $(OBJ1)* $(OBJ2)*

mrproper : clean
	rm -rf $(EXEC1) $(EXEC2)
