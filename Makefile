NAME    = webserv

CXX     = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDE = -Iinclude

SRC     = src/network/main.cpp     \
          src/network/PollLoop.cpp \
          src/network/Listener.cpp \
          src/network/Client.cpp   \
          src/http/Request.cpp     \
          src/http/Response.cpp    \
          src/app/Config.cpp       \
          src/app/Handlers.cpp     \
          src/app/Cgi.cpp

OBJ     = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
