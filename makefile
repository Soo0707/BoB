NAME := test

CXX := g++
CXXFLAGS :=	-Ibob -Icatch2 -std=c++20 -Wshadow -Wconversion -march=native
DEBUGFLAGS := -g -D_GLIBCXX_DEBUG
SANITIZEFLAGS := -fsanitize=undefined,leak,address
THREADSANFLAG := -fsanitize=thread

CACHE := /mnt/tmpfs/cache
SRCS := catch2/catch_amalgamated.cpp $(wildcard tests/*.cpp)
OBJS := $(addprefix $(CACHE)/, $(SRCS:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) $(SANITIZEFLAGS) $(OBJS) -o $(NAME)

$(CACHE)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) $(SANITIZEFLAGS) -c $< -o $@

