NAME = chip8

CPP = clang++
LIBS = -L/opt/homebrew/lib -lSDL2
INC = -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE

SRCS = 	chip8.cpp \
		debug.cpp \
		loop.cpp \
		main.cpp

OBJ_DIRECTORY=./obj
OBJS = $(SRCS:.cpp=.o)

OPTS=
DEBUG_OPTS=-DDEBUG

all:  $(NAME)

$(NAME):
	$(CPP) $(SRCS) $(OPTS) $(INC) -c $(SRCS)
	$(CPP) $(OBJS) $(OPTS) $(LIBS) -o $(NAME)
	ls $(OBJS)

debug:
	$(CPP) $(SRCS) $(OPTS) $(INC) -c $(SRCS) -DDEBUG
	$(CPP) $(OBJS) $(OPTS) $(LIBS) -o $(NAME)
	ls $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all 
