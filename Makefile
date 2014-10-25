CXX=g++-4.8
override CXXFLAGS+=-Imatrix/include -Irapidjson/include -g -gdwarf-2 -Wall -Wunused -Wextra -pedantic -std=c++11
MAKE=make
LDFLAGS+=-Lmatrix/lib -lrgbmatrix -lrt -lm -lpthread

ifneq (,$(findstring -DEMULATE_LEDBOARD,$(CXXFLAGS)))
LDFLAGS+=-lSDL
endif

unexport CXXFLAGS

OBJS=main.o util.o Emulator.o Fill.o PixelMap.o Conway.o

board_controller: $(OBJS) matrix/lib/librgbmatrix.a
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

main.o: main.cpp
	$(CXX) -c $(CXXFLAGS) $<

%.o: %.cpp %.h
	$(CXX) -c $(CXXFLAGS) $<

matrix/lib/librgbmatrix.a:
	$(MAKE) -C matrix/lib librgbmatrix.a

clean:
	rm -f board_controller $(OBJS)
	$(MAKE) -C matrix clean

.PHONY: clean
