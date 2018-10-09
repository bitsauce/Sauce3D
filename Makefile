CC      = g++
CFLAGS  = -Wall -fsigned-char -std=c++11 -D__LINUX__
LDFLAGS = 

BUILD_DIR = ./build/
BINARY    = $(BUILD_DIR)Sauce3D

SOURCE_DIR    = ./source/
SOURCE_FILES := $(shell find $(SOURCE_DIR) -name '*.cpp')

HEADER_DIR    = ./include/
HEADER_FILES := $(shell find $(HEADER_DIR) -name '*.h')

OBJECT_FILES := $(addprefix $(BUILD_DIR)/,$(SOURCE_FILES:%.cpp=%.o))

SDL2_CONFIG      = sdl2-config
SDL2_INCLUDE_DIR = /usr/include/SDL2/
GL3W_INCLUDE_DIR = ./3rdparty/gl3w/include/

MKDIR = mkdir -p

CFLAGS += -I$(HEADER_DIR) -I$(SDL2_INCLUDE_DIR) -I$(GL3W_INCLUDE_DIR)

$(BUILD_DIR)/%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(dir $<) -c $< -o $@

all: $(OBJECT_FILES)
	$(MKDIR) $(BUILD_DIR)
	$(info find $(SOURCE_DIR) -type d -exec mkdir -p $(BUILD_DIR){} \;)
	$(shell find $(SOURCE_DIR) -type d -exec mkdir -p $(BUILD_DIR){} \;)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECT_FILES) -o $(BINARY)

install-dependencies:
	ifeq ("$(wildcard $(SDL2_CONFIG))","")
		$(info Installing SLD2...)
		sudo apt-get install -y libsdl2-2.0 libsdl2-dev libsdl2-image-2.0-0 libsdl2-image-dev
		ifneq ("$(wildcard $(SDL2_CONFIG))","")
			$(error Failed to download SDL2!)
		endif
	endif
	$(info -- SLD2 Installed)