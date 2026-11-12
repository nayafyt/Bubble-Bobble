# ---------------------------------------------------------------------------
# Bubble Bobble - Linux / macOS build
#
#   ./scripts/setup-deps.sh    fetch SGG + its dependencies into third_party/
#   make                       build bin/BubbleBobble
#   make run                   build and play
#   make clean                 remove build output
#
# Windows users want Stage4.sln instead; see README.md.
# ---------------------------------------------------------------------------

CXX      ?= g++
CXXSTD   ?= -std=c++17
OPTFLAGS ?= -O2

GAME_DIR  := BubbleBobbleGame
SGG_DIR   := third_party/sgg
LOCAL     := third_party/local/usr
BUILD_DIR := build
BIN_DIR   := bin
TARGET    := $(BIN_DIR)/BubbleBobble

# The game includes <sgg/graphics.h>; -I$(SGG_DIR) resolves it to the header
# belonging to the library sources we actually compile, so the two can never
# drift apart. (include/ holds an older vendored copy, kept for the MSVC build.)
INCLUDES := -I$(GAME_DIR) -I$(SGG_DIR)

# --- dependency discovery --------------------------------------------------
# Prefer the system packages; fall back to the root-free prefix that
# scripts/setup-deps.sh unpacks into third_party/local.
PKG_CONFIG ?= pkg-config

ifneq ($(wildcard $(LOCAL)/include/SDL2/SDL.h),)
  DEP_CFLAGS := -I$(LOCAL)/include -I$(LOCAL)/include/SDL2 \
                -I$(LOCAL)/include/freetype2 \
                -I$(LOCAL)/include/$(shell $(CXX) -dumpmachine)
  # --disable-new-dtags emits DT_RPATH rather than DT_RUNPATH. Unlike
  # DT_RUNPATH, DT_RPATH is also consulted for indirect dependencies, which is
  # what lets SDL2_mixer find its decoders (libxmp, libopusfile, ...) inside
  # our prefix.
  DEP_LIBS   := -L$(LOCAL)/lib/$(shell $(CXX) -dumpmachine) -L$(LOCAL)/lib \
                -Wl,--disable-new-dtags \
                -Wl,-rpath,$(abspath $(LOCAL))/lib/$(shell $(CXX) -dumpmachine) \
                -Wl,-rpath,$(abspath $(LOCAL))/lib \
                -lSDL2 -lSDL2_mixer -lGLEW -lfreetype -lGL -lGLU
  DEP_SOURCE := third_party/local
else ifneq ($(shell $(PKG_CONFIG) --exists sdl2 SDL2_mixer glew freetype2 && echo yes),)
  DEP_CFLAGS := $(shell $(PKG_CONFIG) --cflags sdl2 SDL2_mixer glew freetype2)
  DEP_LIBS   := $(shell $(PKG_CONFIG) --libs sdl2 SDL2_mixer glew freetype2) -lGL -lGLU
  DEP_SOURCE := system packages
else
  DEP_SOURCE := MISSING
endif

CXXFLAGS := $(CXXSTD) $(OPTFLAGS) $(INCLUDES) $(DEP_CFLAGS) $(EXTRA_CXXFLAGS)
LDFLAGS  := $(DEP_LIBS) $(EXTRA_LDFLAGS)

# --- sources ---------------------------------------------------------------
GAME_SRCS := $(sort $(wildcard $(GAME_DIR)/*.cpp))
GAME_OBJS := $(patsubst $(GAME_DIR)/%.cpp,$(BUILD_DIR)/game/%.o,$(GAME_SRCS))

SGG_SRCS := $(sort $(wildcard $(SGG_DIR)/sgg/*.cpp))
SGG_OBJS := $(patsubst $(SGG_DIR)/sgg/%.cpp,$(BUILD_DIR)/sgg/%.o,$(SGG_SRCS))
SGG_LIB  := $(BUILD_DIR)/libsgg.a

# SGG is third-party code: build it quietly rather than drowning our own
# warnings in it.
SGG_CXXFLAGS := $(CXXSTD) $(OPTFLAGS) -I$(SGG_DIR) $(DEP_CFLAGS) -w

.PHONY: all run clean distclean deps check-deps

all: check-deps $(TARGET)

check-deps:
ifeq ($(DEP_SOURCE),MISSING)
	@echo "Dependencies not found. Run ./scripts/setup-deps.sh first." >&2
	@exit 1
endif
ifeq ($(SGG_SRCS),)
	@echo "SGG sources missing from $(SGG_DIR). Run ./scripts/setup-deps.sh first." >&2
	@exit 1
endif
	@echo "Using dependencies from: $(DEP_SOURCE)"

deps:
	@./scripts/setup-deps.sh

$(TARGET): $(GAME_OBJS) $(SGG_LIB) | $(BIN_DIR)
	$(CXX) $(GAME_OBJS) $(SGG_LIB) -o $@ $(LDFLAGS)
	@echo "Built $@"

$(SGG_LIB): $(SGG_OBJS)
	$(AR) rcs $@ $^

$(BUILD_DIR)/game/%.o: $(GAME_DIR)/%.cpp | $(BUILD_DIR)/game
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/sgg/%.o: $(SGG_DIR)/sgg/%.cpp | $(BUILD_DIR)/sgg
	$(CXX) $(SGG_CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/game $(BUILD_DIR)/sgg $(BIN_DIR):
	@mkdir -p $@

# The game loads its assets from a relative "assets/" path, so it has to run
# with bin/ as the working directory.
run: all
	cd $(BIN_DIR) && ./$(notdir $(TARGET))

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

distclean: clean
	rm -rf third_party

-include $(GAME_OBJS:.o=.d)
