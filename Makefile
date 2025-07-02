###############################################################################
## ARGUMENTS

NAME = ircserv
CXX = c++
CXXFLAGS = -g -Wall -Wextra -Werror -std=c++98
INCLUDE = -I./

SRC_DIR = src/
OBJ_DIR = build/

SRC = main.cpp \
      Server.cpp \
      Client.cpp \
      utils/Utils.cpp \
      utils/SigHandler.cpp \
			commands/Channel.cpp \
			commands/invite.cpp \
			commands/join.cpp \
			commands/quit.cpp \
			commands/kick.cpp \
			commands/mode.cpp \
			commands/nick.cpp \
			commands/part.cpp \
			commands/pass.cpp \
			commands/privmsg.cpp \
			commands/topic.cpp \
			commands/user.cpp \
			commands/quoteBot.cpp \
			commands/Quote.cpp \
			commands/ping.cpp \



# Construction des chemins complets
SRCS = $(addprefix $(SRC_DIR), $(SRC))
OBJS = $(SRCS:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

###############################################################################
## RULES

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Creating $(NAME) ......."
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	@echo "CLEAN all .o files ......."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "FCLEAN binary file ......."
	@rm -f $(NAME)

re: fclean all
	@echo "All erased and re-compiled ......."

.PHONY: all clean fclean re

