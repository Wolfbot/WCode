PROJ_NAME = wolfbot
#CXX = /root/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/bin/arm-none-linux-gnueabi-g++
CXX = g++
IDIR = include
VECTFLAGS = #-march=armv7-a -mtune=cortex-a8  -mcpu=cortex-a8 -mfloat-abi=hard  -mfpu=neon -ftree-vectorize  -fassociative-math -ffast-math -fsingle-precision-constant -mvectorize-with-neon-quad -ftree-vectorizer-verbose=2 -funroll-loops -fprefetch-loop-arrays  # -funsafe-math-optimizations
CXXFLAGS = -Wall -g -pipe -O3 `pkg-config opencv --cflags` -I./include $(VECTFLAGS) -I/usr/include/arm-linux-gnueabihf -fpermissive # -fvariable-expansion-in-unroller  #-fgcse-las #-funsafe-loop-optimizations -Wunsafe-loop-optimizations -fno-inline-functions 
LIBS = -lm -lrt `pkg-config --libs opencv` -lpthread
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.s,%.o,$(wildcard *.s))
$(PROJ_NAME): $(OBJFILES) 
#	echo $(OBJFILES)
	$(CXX) -o $(PROJ_NAME) $(OBJFILES) $(LIBS)
	rm -f *.o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
%.s: %.cpp
	$(CXX) $(CXXFLAGS) -S -o $@ $<
%.lst: %.cpp
	$(CXX) $(CXXFLAGS) -Wa,-adhln $(LIBS) $< > $@
clean:
	rm -f *.o *.lst *.s
