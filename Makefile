CXX=g++-4.8
CXXFLAGS=-Imatrix/include -Irapidjson/include -g -Wall -Wunused -Wextra -pedantic -std=c++11
MAKE=make
LDFLAGS+=-Lmatrix/lib -lrgbmatrix -lrt -lm -lpthread

OBJS=main.o util.o Fill.o PixelMap.o

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
