CC       = g++
CXXFLAGS = -Wall -fsigned-char -std=c++11 -D__LINUX__ -D_REENTRANT -fPIC
LDFLAGS  = -lGLEW -lGLU -lGL -L/usr/lib/x86_64-linux-gnu -lSDL2 -lSDL2_image  -L../../lib/

BUILD_DIR_RELEASE = ./build/linux/release/
BUILD_DIR_DEBUG   = ./build/linux/debug/
BINARY            = minimal

SOURCE_DIR    = ./Source/
SOURCE_FILES := $(shell find $(SOURCE_DIR) -name '*.cpp')

HEADER_DIR    = ./Source/
HEADER_FILES := $(shell find $(HEADER_DIR) -name '*.h')

OBJECT_FILES_RELEASE := $(addprefix $(BUILD_DIR_RELEASE),$(SOURCE_FILES:%.cpp=%.o))
OBJECT_FILES_DEBUG := $(addprefix $(BUILD_DIR_DEBUG),$(SOURCE_FILES:%.cpp=%.o))

SDL2_CONFIG      = sdl2-config
SDL2_INCLUDE_DIR = /usr/include/SDL2/

MKDIR = mkdir -p

CXXFLAGS += -I$(HEADER_DIR) -I$(SDL2_INCLUDE_DIR) -I../../include

$(BUILD_DIR_RELEASE)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CXXFLAGS) -I$(dir $<) -c $< -o $@ $(LDFLAGS)

$(BUILD_DIR_DEBUG)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CXXFLAGS) -I$(dir $<) -c $< -o $@ $(LDFLAGS)

.PHONY: debug
debug: CXXFLAGS += -DDEBUG -g
debug: LDFLAGS += -lsauce3d_d
debug: build-debug
build-debug: $(OBJECT_FILES_DEBUG)
	$(CC) $(CXXFLAGS) $(OBJECT_FILES_DEBUG) -o $(BUILD_DIR_DEBUG)$(BINARY) $(LDFLAGS)

.PHONY: release
release: CXXFLAGS += -O2
release: LDFLAGS += -lsauce3d
release: build-release
build-release: $(OBJECT_FILES_RELEASE)
	$(CC) $(CXXFLAGS) $(OBJECT_FILES_RELEASE) -o $(BUILD_DIR_RELEASE)$(BINARY) $(LDFLAGS)

all: debug
all: release

.PHONY: clean
clean:
	rm -r -f $(BUILD_DIR_DEBUG)
	rm -r -f $(BUILD_DIR_RELEASE)

.PHONY: install-dependencies
install-dependencies:
	ifeq ("$(wildcard $(SDL2_CONFIG))","")
		$(info Installing SLD2...)
		sudo apt-get install -y libsdl2-2.0 libsdl2-dev libsdl2-image-2.0-0 libsdl2-image-dev
		ifneq ("$(wildcard $(SDL2_CONFIG))","")
			$(error Failed to download SDL2!)
		endif
	endif
	$(info -- SLD2 Installed)
