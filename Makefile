NAME		:= app
RUN_CMD		:= ./$(NAME) 

CFLAGS		+= -Wall -Wextra -std=c++17 -g

IFLAGS		:=	-Iinclude

SRC_DIR		:=	src
SRC_ENTRY	:=	main.cpp
SRCS		:=	Server.cpp \
				Socket.cpp \
				HttpServer.cpp \
				CGI.cpp			\
				HttpObjects/HttpRequest.cpp \
				HttpObjects/HttpResponse.cpp \
				RequestHandlers/GetRequestHandler.cpp \
				RequestHandlers/PostRequestHandler.cpp \
				RequestHandlers/BadRequestHandler.cpp \
				RequestHandlers/RequestHandler.cpp \
				RequestHandlers/HandlerFactory.cpp \
				logging/Logger.cpp


HEADER_DIR	:=	include
HEADERS 	:=	Server.hpp \
				Socket.hpp \
				meta.hpp \
				HttpServer.hpp \
				CGI.hpp			\
				HttpRequest.hpp \
				HttpResponse.hpp \
				GetRequestHandler.hpp \
				PostRequestHandler.hpp \
				RequestHandler.hpp	\
				HandlerFactory.hpp \

OBJ_DIR		:=	obj


SRCS 		:=	$(addprefix $(SRC_DIR)/, $(SRCS))
HEADERS 	:=	$(addprefix $(HEADER_DIR)/, $(HEADERS))


OBJS 		:=	$(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRCS))
OBJ_DIRS 	:=	$(dir $(OBJS))

.PHONY: make_libs fclean

all: 
	$(MAKE) $(NAME) -j4

$(NAME): $(OBJS) $(SRC_DIR)/$(SRC_ENTRY)
	$(CXX) $(SRC_DIR)/$(SRC_ENTRY) $(OBJS) $(CFLAGS) $(IFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIRS)
	$(CXX) $(CFLAGS) $(IFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

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
