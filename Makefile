CXX = g++

CXXFLAGS = -O2 -Wall -Wextra -pedantic-errors -Wold-style-cast
CXXFLAGS += -std=c++11
LDFLAGS = -g
LDLIBS = -lSDL2
TESTLIBS = -lgtest -lgtest_main -lpthread

SRCDIR = src
TESTDIR = test

SRCS := $(wildcard $(SRCDIR)/*/*.cpp)
SRCS := $(filter-out src/debug/%.cpp, $(SRCS))
SRCSDBG := $(SRCS) $(wildcard $(SRCDIR)/debug/*.cpp)
SRCSTEST := $(wildcard $(TESTDIR)/*/*.cpp)

OBJS := $(subst .cpp,.o,$(SRCS))
OBJSDBG := $(subst .cpp,.o,$(SRCSDBG))
OBJSTEST := $(subst .cpp,.o,$(SRCSTEST))

PROGS = geemuboi geemuboi-dbg geemuboi-test

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
$(shell mkdir -p $(DEPDIR)/src/core/ >/dev/null)
$(shell mkdir -p $(DEPDIR)/src/geemuboi/ >/dev/null)
$(shell mkdir -p $(DEPDIR)/src/video/ >/dev/null)
$(shell mkdir -p $(DEPDIR)/src/debug/ >/dev/null)
$(shell mkdir -p $(DEPDIR)/src/input/ >/dev/null)

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

all: $(PROGS)

geemuboi: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o geemuboi $(OBJS) $(LDLIBS)

geemuboi-dbg: CXXFLAGS += -DDEBUG
geemuboi-dbg: $(OBJSDBG)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o geemuboi-dbg $(OBJSDBG) $(LDLIBS)

geemuboi-test: $(OBJSTEST)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o geemuboi-test $(OBJSTEST) $(TESTLIBS)

clean:
	rm -f $(PROGS)
	rm -f src/*/*.o
	rm -f test/*/*.o
	rm -rf .d

# replace built in rule with added dependency support
%.o: %.cpp
%.o: %.cpp $(DEPDIR)/*/*.d
		$(COMPILE.cpp) $(OUTPUT_OPTION) $<
		@$(POSTCOMPILE)

.PHONY: all clean

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))
