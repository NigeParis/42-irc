###############################################################################
## ARGUMENTS

NAME= ircserv
CXXFLAGS= -g -Wall -Wextra -Werror -std=c++98

SRCS=$(addprefix $(SRC_DIR), $(SRC))
OBJ=$(SRC:.cpp=.o)
OBJS=$(addprefix $(OBJ_DIR), $(OBJ))

INCLUDE = -I./

CXX=c++
###############################################################################
## SOURCES 


SRC_DIR = src/
SRC = main.cpp Server.cpp  SigHandler.cpp

SRC +=  Channel.cpp Client.cpp Utils.cpp

OBJ_DIR = build/

###############################################################################
## RULES

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Creating FILE .......\n"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@ > /dev/null

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE) > /dev/null

clean: 
	@echo "CLEAN all .o files .......\n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "FCLEAN all .o et .a files .......\n"
	@rm -f $(NAME) > /dev/null

re: fclean all
	@echo "All erased and re-compiled .......\n"		

.PHONY: all clean fclean re
