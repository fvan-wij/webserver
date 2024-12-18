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
				CGI.cpp			\
				HttpObjects/HttpRequest.cpp \
				logging/Logger.cpp					\
				Config/ConfigParser.cpp				\
				Config/print_config.cpp				\
				Utility.cpp							\
				ConnectionManager.cpp				\
				ClientHandler.cpp				\
				HttpListener.cpp					\
				FileHandler.cpp						\
				Timer.cpp							\

TEST_SRCS	:=	test/HttpRequestTest.cpp \
				test/TestLogger.cpp

HEADER_DIR	:=	include

HEADERS 	:=  Socket.hpp \
				meta.hpp \
				CGI.hpp			\
				HttpRequest.hpp \
				Utility.hpp			\
				Config.hpp			\
				ConfigParser.hpp	\
				Action.hpp			\
				ClientHandler.hpp \
				HttpListener.hpp	\
				FileHandler.hpp		\
				Timer.hpp			\
				HttpExceptions.hpp	\

TEST_HEADERS:=	test.hpp		\
				TestLogger.hpp

OBJ_DIR		:=	obj

SRCS 		:=	$(addprefix $(SRC_DIR)/, $(SRCS))
HEADERS 	:=	$(addprefix $(HEADER_DIR)/, $(HEADERS))

TEST_SRCS	:=	$(addprefix $(SRC_DIR)/, $(TEST_SRCS))
TEST_HEADERS:=	$(addprefix $(HEADER_DIR)/, $(TEST_HEADERS))

OBJS 		:=	$(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRCS))
OBJ_DIRS 	:=	$(dir $(OBJS))
TEST_OBJS	:=	$(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(TEST_SRCS))
TEST_OBJ_DIRS:=	$(dir $(TEST_OBJS))

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

test_dir:
	mkdir -p $(TEST_OBJ_DIRS)

test_cpp: test_dir $(TEST_OBJS) $(OBJS)
	mkdir -p $(OBJ_DIRS)
	$(CXX) $(SRC_DIR)/test/test.cpp $(TEST_OBJS) $(OBJS) $(CFLAGS) $(IFLAGS) -o test_cpp


test: all
	pytest pytests/

debug:
	$(MAKE) DEBUG=1
