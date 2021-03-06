CC       = g++
CXXFLAGS = -Wall -fsigned-char -std=c++11 -D__LINUX__ -D_REENTRANT -fPIC
LDFLAGS  = -lGLEW -lGLU -lGL -L/usr/lib/x86_64-linux-gnu -lSDL2 -lSDL2_image

BUILD_DIR_RELEASE = ./build/linux/release/
BUILD_DIR_DEBUG   = ./build/linux/debug/
LIBRARY_DIR       = ./lib/
SAUCE_LIB_RELEASE = libsauce3d.so
SAUCE_LIB_DEBUG   = libsauce3d_d.so

SOURCE_DIR    = ./source/
SOURCE_FILES := $(shell find $(SOURCE_DIR) -name '*.cpp')

HEADER_DIR    = ./include/
HEADER_FILES := $(shell find $(HEADER_DIR) -name '*.h')

OBJECT_FILES_RELEASE := $(addprefix $(BUILD_DIR_RELEASE),$(SOURCE_FILES:%.cpp=%.o))
OBJECT_FILES_DEBUG := $(addprefix $(BUILD_DIR_DEBUG),$(SOURCE_FILES:%.cpp=%.o))

SDL2_CONFIG      = sdl2-config
SDL2_INCLUDE_DIR = /usr/include/SDL2/

MKDIR = mkdir -p

CXXFLAGS += -I$(HEADER_DIR) -I$(SDL2_INCLUDE_DIR)

$(BUILD_DIR_RELEASE)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CXXFLAGS) -I$(dir $<) -c $< -o $@ $(LDFLAGS)

$(BUILD_DIR_DEBUG)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CXXFLAGS) -I$(dir $<) -c $< -o $@ $(LDFLAGS)

.PHONY: debug
debug: CXXFLAGS += -DDEBUG -g
debug: build-debug
build-debug: $(OBJECT_FILES_DEBUG)
	$(MKDIR) $(LIBRARY_DIR)
	$(CC) $(CXXFLAGS) -shared $(OBJECT_FILES_DEBUG) -o $(LIBRARY_DIR)$(SAUCE_LIB_DEBUG) $(LDFLAGS)

.PHONY: release
release: CXXFLAGS += -O2
release: build-release
build-release: $(OBJECT_FILES_RELEASE)
	$(MKDIR) $(LIBRARY_DIR)
	$(CC) $(CXXFLAGS) -shared $(OBJECT_FILES_RELEASE) -o $(LIBRARY_DIR)$(SAUCE_LIB_RELEASE) $(LDFLAGS)

all: debug
all: release

.PHONY: clean
clean:
	rm -r -f $(BUILD_DIR_DEBUG)
	rm -r -f $(BUILD_DIR_RELEASE)
	rm -r -f $(LIBRARY_DIR)

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
