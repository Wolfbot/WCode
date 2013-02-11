PROJ_NAME= Wolfbot
CC 		 = g++
SRCEXT   = cpp
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SRCS    := $(shell find $(SRCDIR) -name '*.$(SRCEXT)')
SRCDIRS := $(shell find . -name '*.$(SRCEXT)' -exec dirname {} \; | uniq)
OBJS    := $(patsubst %.$(SRCEXT),$(OBJDIR)/%.o,$(SRCS))


INCLUDES = -I./inc

VECTFLAGS = -ftree-vectorize  -fassociative-math -ffast-math -fsingle-precision-constant -mvectorize-with-neon-quad -ftree-vectorizer-verbose=2 -funroll-loops # -funsafe-math-optimizations

CFLAGS   = $(INCLUDES) -Wall -g -pipe -O3 -march=armv7-a -mtune=cortex-a8  -mcpu=cortex-a8 -mfloat-abi=hard  -mfpu=neon $(VECTFLAGS) -I/usr/include/arm-linux-gnueabihf -fprefetch-loop-arrays `pkg-config opencv --cflags` # -fvariable-expansion-in-unroller  #-fgcse-las #-funsafe-loop-optimizations -Wunsafe-loop-optimizations -fno-inline-functions 
LDFLAGS  = -lm -lrt `pkg-config --libs opencv` -lpthread 


.PHONY: all clean distclean
all: $(BINDIR)/$(PROJ_NAME)

$(BINDIR)/$(PROJ_NAME): buildrepo $(OBJS)
	@mkdir -p `dirname $@`
	@echo "Linking $@..."
	@$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: %.$(SRCEXT)
	@echo "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) -r $(OBJDIR)

distclean: clean
	$(RM) -r $(BINDIR)

buildrepo:
	@$(call make-repo)

define make-repo
   for dir in $(SRCDIRS); \
   do \
	mkdir -p $(OBJDIR)/$$dir; \
   done
endef


# usage: $(call make-depend,source-file,object-file,depend-file)
define make-depend
  $(CC) -MM       \
        -MF $3    \
        -MP       \
        -MT $2    \
        $(CFLAGS) \
        $1
endef