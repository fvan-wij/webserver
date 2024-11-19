NAME		:= app
RUN_CMD		:= ./$(NAME) conf/test.conf

ifdef DEBUG
	CFLAGS		:= -Wall -Wextra -std=c++17 -g -fsanitize=address -pedantic
else
	CFLAGS		:= -Wall -Wextra -std=c++17 -pedantic
endif

IFLAGS		:=	-Iinclude

SRC_DIR		:=	src
SRC_ENTRY	:=	main.cpp
SRCS		:=	Socket.cpp \
				HttpProtocol.cpp \
				CGI.cpp			\
				HttpObjects/HttpRequest.cpp \
				HttpObjects/HttpResponse.cpp \
				RequestHandlers/GetRequestHandler.cpp \
				RequestHandlers/PostRequestHandler.cpp \
				RequestHandlers/DeleteRequestHandler.cpp \
				RequestHandlers/BadRequestHandler.cpp \
				RequestHandlers/RequestHandler.cpp 	\
				RequestHandlers/HandlerFactory.cpp 	\
				logging/Logger.cpp					\
				Config/ConfigParser.cpp				\
				Config/print_config.cpp				\
				Utility.cpp							\
				ConnectionManager.cpp				\
				ConnectionInfo.cpp					\
				ClientHandler.cpp				\
				HttpListener.cpp					\
				FileHandler.cpp						\
				Timer.cpp							\


HEADER_DIR	:=	include
HEADERS 	:=  Socket.hpp \
				meta.hpp \
				HttpProtocol.hpp \
				CGI.hpp			\
				HttpRequest.hpp \
				HttpResponse.hpp \
				GetRequestHandler.hpp \
				PostRequestHandler.hpp \
				DeleteRequestHandler.hpp \
				RequestHandler.hpp	\
				HandlerFactory.hpp	\
				Utility.hpp			\
				Config.hpp			\
				ConfigParser.hpp	\
				Action.hpp			\
				ClientHandler.hpp \
				HttpListener.hpp	\
				FileHandler.hpp		\
				Timer.hpp			\

OBJ_DIR		:=	obj


SRCS 		:=	$(addprefix $(SRC_DIR)/, $(SRCS))
HEADERS 	:=	$(addprefix $(HEADER_DIR)/, $(HEADERS))


OBJS 		:=	$(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRCS))
OBJ_DIRS 	:=	$(dir $(OBJS))

.PHONY: make_libs fclean

all:
	$(MAKE) $(NAME) -j4
	@mkdir -p ./var/www/uploads

$(NAME): $(OBJS) $(SRC_DIR)/$(SRC_ENTRY)
	$(CXX) $(SRC_DIR)/$(SRC_ENTRY) $(OBJS) $(CFLAGS) $(IFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIRS)
	$(CXX) $(CFLAGS) $(IFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -rf ./var/www/uploads

fclean: clean
	rm -f $(NAME)
	rm -rf $(NAME).dSYM

re: fclean all

run: all
	$(RUN_CMD)

compile_commands: fclean
	$(MAKE) | compiledb

norm:
	norminette include src

cgi:
	mkdir -p ~/.local/bin
	gcc sleep_echo_prog.c -o ~/.local/bin/sleep_echo_var

test: all
	pytest pytests/

debug:
	$(MAKE) DEBUG=1

test: all
	pytest pytests/
