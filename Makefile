########################################################################
####################### Makefile Template ##############################
########################################################################

APPNAME = Assignment4
DEBUG = -g
VERBOSE = -v
CXX = g++

# For defining platform specific flags
PLATFORM = $(shell uname -s)
# Handle special case for Windows 10 and 11 uname difference
ifeq ($(PLATFORM), Windows_NT)
PLATFORM = Windows
else ifeq ($(shell echo $(PLATFORM) | cut -c1-7), MSYS_NT)
PLATFORM = Windows
endif

# Windiows definitions
ifeq ($(PLATFORM), Windows)
	APP_NAME = $(APPNAME).exe
	GLM_PATH = $(VULKAN_SDK)/include
	GLFW_PATH = $(VULKAN_SDK)/glfw-3.4.bin.WIN64/include
	GLFW_LIB_PATH = $(VULKAN_SDK)/glfw-3.4.bin.WIN64/lib-mingw-w64
	LDFLAGS = -L$(VULKAN_SDK)/Lib -L$(GLFW_LIB_PATH) -lvulkan-1 -lglfw3 -lgdi32
	RUNSCRIP = ./compile-win.bat
	DEFINES = __WINDOWS__

# MacOS definitions
else ifeq ($(PLATFORM), Darwin)
	APP_NAME = $(APPNAME)
	GLM_PATH = $(VULKAN_SDK)/include
	GLFW_PATH = $(VULKAN_SDK)/glfw-3.4.bin.MACOS/include
	GLFW_LIB_PATH = $(VULKAN_SDK)/glfw-3.4.bin.MACOS/lib-arm64
	LDFLAGS = -L$(VULKAN_SDK)/lib -L$(GLFW_LIB_PATH) -rpath $(VULKAN_SDK)/lib -lvulkan -lglfw3 -framework Cocoa -framework IOKit
	RUNSCRIP = ./compile-unx.bat
	DEFINES = __DARWIN__

# Linux definitions
else ifeq ($(PLATFORM), Linux)
	APP_NAME = $(APPNAME)
	GLM_PATH = /usr/include/glm-master
	GLFW_PATH = /usr/include/GLFW
	GLFW_LIB_PATH = /usr/lib/x86_64-linux-gnu
	LDFLAGS = -L$(VULKAN_SDK)/lib -L$(GLFW_LIB_PATH) -lvulkan -lglfw
	RUNSCRIP = ./compile-unx.bat
	DEFINES = __LINUX__
endif

CFLAGS = -std=c++17 $(DEBUG) -I. -I$(VULKAN_SDK)/include -I$(GLM_PATH) -I$(GLFW_PATH)
include LOCAL_SRCS

# Object files
OBJECTS = $(CPPSRCS:.cpp=.o)

# Default target
all: $(APPNAME)

# Link object files to create the executable
$(APPNAME): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)
	@echo Done!

# Compile C++ files
%.o: %.cpp
	$(CXX) -D$(DEFINES) $(CFLAGS) -c $< -o $@

shader:
ifneq ($(PLATFORM), Windows)
	chmod 755 $(RUNSCRIP)
endif
	$(RUNSCRIP)

run: $(APP_NAME)
	./$(APP_NAME)

clean:
	rm -f $(OBJECTS) $(APP_NAME) shaders/*.spv

test:
	@echo $(PLATFORM)
