BINDIR = bin
OBJDIR = obj
SRCDIR = src
INCLUDE = include

TARGET = voxel

CXX ?= g++
CXXFLAGS = -Wall -Wextra -Wno-unused-parameter -std=c++11

# Chooses the proper compiler flags to use based on debug
# or release. By default release, use `make debug=1 ...`.
ifeq ($(debug), 1)
	CXXFLAGS += -O0 -ggdb -D DEBUG
else
	CXXFLAGS += -O3
endif

# Adds the -no-integrated-as flag if using clang
ifeq ($(CXX), clang)
    CXXFLAGS += -no-integrated-as
endif

ifeq ($(testing), 1)
	CXXFLAGS += -D TESTING
endif

CFLAGS += $(extra)

HEADERS = $(shell find $(INCLUDE)/ -name '*.h')
SRC = $(shell find $(SRCDIR)/ -name '*.cpp')
ASM = $(shell find $(SRCDIR)/ -name '*.S')
SRCOBJ = $(subst .cpp,.cpp.o,$(subst $(SRCDIR)/,$(OBJDIR)/,$(SRC)))
ASMOBJ = $(subst .S,.S.o,$(subst $(SRCDIR)/,$(OBJDIR)/,$(ASM)))
BINPATH = $(addprefix $(BINDIR)/, $(TARGET))

default: $(OBJDIR) $(BINDIR) $(BINPATH)

$(OBJDIR):
	@mkdir $@

$(BINDIR):
	@mkdir $@

$(BINPATH): $(SRCOBJ) $(ASMOBJ)
	$(CXX) $(LDFLAGS) $(SRCOBJ) $(ASMOBJ) -o $(BINPATH)

$(OBJDIR)/%.cpp.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c $< -o $@

$(OBJDIR)/%.S.o: $(SRCDIR)/%.S $(HEADERS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c $< -o $@

.PHONY: clean_bin
clean_bin:
	rm -rf $(BINDIR)

.PHONY: cleanobj
clean_obj:
	rm -rf $(OBJDIR)

.PHONY: clean
clean: clean_bin clean_obj
	
