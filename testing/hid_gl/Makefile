CFLAGS := -Iinclude -Wall -fstack-protector-strong -O2 -pipe \
    -Werror=format-security -I/usr/local/include -I ./include
CXXFLAGS := $(CFLAGS) -std=c++17
LDFLAGS := -L/usr/local/lib 
LIBS := -lglfw -lGL -lX11 -lpthread -ldl -lassimp -lhidapi-libusb

BIN := gltest 
OBJ := src/main.o src/shader.o src/model.o src/glad.o src/ahrs.o

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LIBS) 

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -r $(BIN) $(OBJ)

.PHONY: clean
.SUFFIXES: .cpp .c .o
