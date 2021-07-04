# the compiler: gcc for C program, define as g++ for C++
# for C++ define  CC = g++
CC = g++
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = 

# Release
CFLAGS  += -Ofast

# Debug
#CFLAGS  += -g 

CFLAGS  += -Wall

SRC_DIR = ./src
BUILD_DIR = ./make_build

# the build target executable:
TARGET = $(BUILD_DIR)/test_replay_packet_detection

# define any directories containing header files other than /usr/include
#
INCLUDES = -I./include

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
#LFLAGS = -L../lib
LFLAG = 

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
#LIBS = -lmylib -lm
LIBS =


# define the C source files
SOURCES = $(SRC_DIR)/main.cpp \
        $(SRC_DIR)/replay_detector.cpp \
        $(SRC_DIR)/test_replay_detector.cpp


OBJS = $(SOURCES:$(SRC_DIR)/.cpp=$(BUILD_DIR)/.o)

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all:    $(TARGET)
	@echo  Compiling has been compiled.

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) $(BUILD_DIR)/*.o *~ $(TARGET)

depend: $(SOURCES)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

