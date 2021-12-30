DRV_DIR ?= $(shell pwd)/c2vr
STEAM_DIR ?= ~/.steam/steam

CXX ?= clang++

CXXFLAGS := -Wall -Wextra -fstack-protector-strong -O2 -pipe \
    -Werror=format-security -I/usr/local/include -DDRIVER_SAMPLE_EXPORTS -std=c++17
LDFLAGS :=

OBJ := src/driver.o src/driverlog.o src/hid.o
BIN := $(DRV_DIR)/bin/linux64/driver_c2vr.so

all: $(BIN)

$(BIN): $(OBJ)
	mkdir -p $(DRV_DIR)/bin/linux64
	$(CXX) -shared $(LDFLAGS) -o $@ $(OBJ) 

.cpp.o:
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

register:
	$(STEAM_DIR)/steamapps/common/SteamVR/bin/linux64/vrpathreg adddriver ${DRV_DIR}

clean:
	-rm -r $(BIN) $(OBJ)

.PHONY: clean register 
.SUFFIXES: .cpp .o
