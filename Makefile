CXX=g++-4.8
CXXFLAGS=-I$(RGB_INCDIR) -g -Wall -Wunused -Wextra -pedantic -std=c++11
MAKE=make
RGB_INCDIR=matrix/include
RGB_LIBDIR=matrix/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

OBJS=main.o

board_controller: $(OBJS) $(RGB_LIBDIR)/$(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

main.o: main.cpp
	$(CXX) -c $(CXXFLAGS) $<

%.o: %.cpp %.h
	$(CXX) -c $(CXXFLAGS) $<

$(RGB_LIBDIR)/$(RGB_LIBRARY):
	$(MAKE) -C $(RGB_LIBDIR) $(RGB_LIBRARY)

clean:
	rm -f board_controller
	$(MAKE) -C $(RGB_LIBDIR) clean

.PHONY: clean
