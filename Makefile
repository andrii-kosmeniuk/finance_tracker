CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -g -O2

WX_CONFIG := $(shell command -v wx-config 2>/dev/null)
ifeq ($(strip $(WX_CONFIG)),)
$(error wx-config not found. Install wxWidgets dev package (for Ubuntu: sudo apt install libwxgtk3.2-dev))
endif

MYSQL_CONFIG := $(shell command -v mysql_config 2>/dev/null || command -v mariadb_config 2>/dev/null)
PKG_CONFIG := $(shell command -v pkg-config 2>/dev/null)

WX_CXXFLAGS := $(shell $(WX_CONFIG) --cxxflags)
WX_LIBS := $(shell $(WX_CONFIG) --libs)

ifneq ($(strip $(MYSQL_CONFIG)),)
MYSQL_CFLAGS := $(shell $(MYSQL_CONFIG) --cflags 2>/dev/null)
MYSQL_LIBS := $(shell $(MYSQL_CONFIG) --libs 2>/dev/null)
else
ifneq ($(strip $(PKG_CONFIG)),)
ifneq ($(shell $(PKG_CONFIG) --exists libmariadb && echo yes),)
MYSQL_CFLAGS := $(shell $(PKG_CONFIG) --cflags libmariadb)
MYSQL_LIBS := $(shell $(PKG_CONFIG) --libs libmariadb)
else
ifneq ($(shell $(PKG_CONFIG) --exists mysqlclient && echo yes),)
MYSQL_CFLAGS := $(shell $(PKG_CONFIG) --cflags mysqlclient)
MYSQL_LIBS := $(shell $(PKG_CONFIG) --libs mysqlclient)
endif
endif
endif
endif

ifeq ($(strip $(MYSQL_CFLAGS)),)
$(error MySQL/MariaDB client dev headers not found. Install libmysqlclient-dev or libmariadb-dev)
endif

ifeq ($(strip $(MYSQL_LIBS)),)
$(error MySQL/MariaDB client linker flags not found. Install libmysqlclient-dev or libmariadb-dev)
endif

CPPFLAGS = -I$(INCLUDE_DIR) $(WX_CXXFLAGS) $(MYSQL_CFLAGS)
LDLIBS = $(WX_LIBS) $(MYSQL_LIBS) -lsodium
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
BIN = $(BIN_DIR)/comp
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
