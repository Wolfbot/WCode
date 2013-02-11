PROJ_NAME = wolfbot
CC = g++
VECTFLAGS = -ftree-vectorize  -fassociative-math -ffast-math -funroll-loops # -funsafe-math-optimizations
CXXFLAGS = -Wall -g -pipe -O3  $(VECTFLAGS) -I/usr/include/arm-linux-gnueabihf -fprefetch-loop-arrays  -I/usr/local/include
LIBS = -lm -lrt -lpthread -li2c
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.s,%.o,$(wildcard *.s))
$(PROJ_NAME): $(OBJFILES) 
#	echo $(OBJFILES)
	$(CC) -o $(PROJ_NAME) $(OBJFILES) $(LIBS)
%.o: %.cpp
	$(CC) $(CXXFLAGS) -c -o $@ $<
%.s: %.cpp
	$(CC) $(CXXFLAGS) -S -o $@ $<
%.lst: %.cpp
	$(CC) $(CXXFLAGS) -Wa,-adhln $(LIBS) $< > $@
clean:
	rm -f *.o *.lst *.s
