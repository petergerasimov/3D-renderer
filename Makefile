CXX      = g++
CXXFLAGS = -O3 -Ivendor/minifb/include

ifeq ($(shell uname -s),Darwin)
CXXFLAGS += -I$(shell brew --prefix)/include
LDLIBS    = -framework Cocoa -framework Carbon -framework QuartzCore \
            -framework Metal -framework MetalKit
else
LDLIBS    = -lX11 -lXrandr -lxkbcommon -lGL -lpthread
endif

MINIFB = build/minifb/libminifb.a
SRCS   = main.cpp obj.cpp renderer.cpp
TARGET = renderer

$(TARGET): $(SRCS) *.hpp $(MINIFB)
	$(CXX) $(CXXFLAGS) $(SRCS) $(MINIFB) $(LDLIBS) -o $@

$(MINIFB):
	cmake -S vendor/minifb -B build/minifb -DCMAKE_BUILD_TYPE=Release \
		-DMINIFB_BUILD_EXAMPLES=OFF -DMINIFB_BUILD_TESTS=OFF
	cmake --build build/minifb -j

run: $(TARGET)
	cd assets && ../$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
