CFLAGS := -Iinclude -Wall -fstack-protector-strong -O2 -pipe
LDFLAGS := -L/usr/local/lib -lhidapi-libusb

BIN := hid_test 
OBJ := hid_test.o

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) 

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -r $(BIN) $(OBJ)

.PHONY: clean
.SUFFIXES: .c .o
