NAME =			chip8_emu

SRCS =			\
			emulator.c \

#TYPE: LIB or PROGRAM
TYPE =			PROGAM

#OS: LINUX/OSX or WINDOWS
OS =			LINUX

UNIX_CC =		gcc
WIN_CC =		i686-w64-mingw32-gcc

EXTENTION =		c

CFLAGS =		-Wall -lglut

#LIB_NAMES =		-lfoo -lbar
#LIB_PATH =		./libfoo/ ./libbar/

#LIB_SUPP =		-lm
#LIB_SUPP_INC =		-I ./foo/inc/

SRC_PATH = 		src/
INC_PATH = 		inc/
OBJ_PATH =		obj/

CC =			$(UNIX_CC)
OBJ_NAME = $(SRCS:.$(EXTENTION)=.o)
SRC = $(addprefix $(SRC_PATH), $(SRCS))
OBJ = $(addprefix $(OBJ_PATH), $(OBJ_NAME))
LIB_INC = $(addprefix -I, $(addsuffix includes, $(LIB_PATH)))
LIB_LIB = $(addprefix -L, $(LIB_PATH))
INC = $(addprefix -I, $(INC_PATH))
INC += $(LIB_SUPP_INC)
LDFLAGS = $(LIB) $(LIB_NAMES)
EMPTY =

ifeq ($(OS), WINDOWS)
NAME := $(NAME).exe
CC = $(WIN_CC)
else
endif

all: libs name $(OBJ) done $(NAME)

name :
	@ echo "\033[4;7mCompiling Objects:\033[0m [$(NAME)]"

done :
	@ echo "\n\033[4m\033[95md\033[93mo\033[32mn\033[96me\033[0m \033[91m!\033[0m\n"

$(NAME): $(OBJ)
	@ echo "\033[4;7mCompiling binary:\033[0m [$(NAME)]"
ifeq ($(TYPE), LIB)
	@ ar -rc $(NAME) $(OBJ)
	@ ranlib $(NAME)
else
	@ $(CC) $(OBJ) $(LDFLAGS) $(LIB_LIB) $(LIB_SUPP) -o $(NAME)
endif
	@ echo "\033[4m\033[95md\033[93mo\033[32mn\033[96me\033[0m \033[91m!\033[0m\n"

$(OBJ_PATH)%.o: $(SRC_PATH)%.$(EXTENTION)
	@ printf "\033[37;7m+\033[0m"
	@ mkdir -p $(OBJ_PATH) 2> /dev/null
	@ $(CC) $(CFLAGS) $(LIB_INC) $(INC) -c $< -o $@

ifeq ($(LIB_PATH), $(EMPTY))
else
libs:
	@ $(foreach lib, $(LIB_PATH), make -C $(lib);)
endif

clean:
	@ rm -rf $(OBJ_PATH)
	@ echo "\033[4;7mCleaning:\033[0m [$(NAME)]\n\033[4m\033[95md\033[93mo\033[32mn\033[96me\033[0m \033[91m!\033[0m\n"
ifeq ($(LIB_PATH), $(EMPTY))
else
	@ $(foreach lib, $(LIB_PATH), make clean -C $(lib);)
endif

fclean: clean
	@ rm -f $(NAME)
	@ echo "\033[4;7mFcleaning:\033[0m [$(NAME)]\033[0m\n\033[4m\033[95md\033[93mo\033[32mn\033[96me\033[0m \033[91m!\033[0m\n"
ifeq ($(LIB_PATH), $(EMPTY))
else
	@ $(foreach lib, $(LIB_PATH), make fclean -C $(lib);)
endif

re: fclean all

.PHONY: all clean fclean re libs