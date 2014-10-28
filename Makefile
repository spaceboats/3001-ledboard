override CXXFLAGS+=-Imatrix/include -Irapidjson/include $(shell freetype-config --cflags) -g -gdwarf-2 -Wall -Wunused -Wextra -pedantic -std=c++11
LDFLAGS+=-Lmatrix/lib -lrgbmatrix -lrt -lm -lpthread -lfreetype

ifneq (,$(findstring -DEMULATE_LEDBOARD,$(CXXFLAGS)))
LDFLAGS+=-lSDL
endif

unexport CXXFLAGS

OBJS=main.o util.o lodepng.o Emulator.o Fill.o PixelMap.o Conway.o TextMap.o

board_controller: $(OBJS) matrix/lib/librgbmatrix.a
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

main.o: main.cpp
	$(CXX) -c $(CXXFLAGS) $<

lodepng.o: lodepng/lodepng.cpp lodepng/lodepng.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o: %.cpp %.h
	$(CXX) -c $(CXXFLAGS) $<

matrix/lib/librgbmatrix.a:
	make -C matrix/lib librgbmatrix.a

clean:
	rm -f board_controller $(OBJS)
	make -C matrix clean

.PHONY: clean
