CC = g++
CXXFLAGS = -g -Wall -std=gnu++0x 
CXXLINK = $(CC)
LIBS = -lm

OBJS = cacheSim.o cache.o  
RM = rm -f

cacheSim: $(OBJS)
	$(CXXLINK) -o cacheSim $(OBJS)

cacheSim.o: cacheSim.cpp cache.H
cache.o : cache.cpp cache.H

clean:
	$(RM) cacheSim *.o *.bak *~ "#"* core
