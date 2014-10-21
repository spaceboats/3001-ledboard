CXX=g++-4.8
CXXFLAGS=-g -Wall -Wunused -Wextra -pedantic -std=c++11
MAKE=make
RGB_INCDIR=matrix/include
RGB_LIBDIR=matrix/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-I$(RGB_INCDIR) -L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

3001-ledboard: 3001-ledboard.cpp $(RGB_LIBDIR)/$(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

$(RGB_LIBDIR)/$(RGB_LIBRARY):
	$(MAKE) -C $(RGB_LIBDIR) $(RGB_LIBRARY)

clean:
	rm -f 3001-ledboard
	$(MAKE) -C $(RGB_LIBDIR) clean

.PHONY: clean
