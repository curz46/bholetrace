src = $(wildcard src/*.cpp)
obj = $(patsubst src%,out%,$(src:.cpp=.o))
dep = $(obj:.o=.d))
exe = bin/blkhole

CC = g++
CFLAGS = -I./include
LDFLAGS = -lGL -lGLEW -lglut
          
.PHONY: all
all: pre $(exe)

.PHONY: pre
pre:
	@mkdir -p out
	@mkdir -p bin

.PHONY: run
run: pre $(exe)
	@./$(exe)

$(exe): $(obj)
	$(CC) -o $(exe) $(SOURCE_EXT) $^ $(LDFLAGS)

out/%.o: src/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
