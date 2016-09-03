CXX = g++

CXXFLAGS = -O2 -Wall -Wextra -pedantic-errors -Wold-style-cast
CXXFLAGS += -std=c++11
LDFLAGS = -g
LDLIBS = -lSDL2

SRCDIR = src

SRCS := $(wildcard $(SRCDIR)/*/*.cpp)
SRCS := $(filter-out src/debug/%.cpp, $(SRCS))
SRCSDBG := $(SRCS) $(wildcard $(SRCDIR)/debug/*.cpp)

OBJS := $(subst .cpp,.o,$(SRCS))
OBJSDBG := $(subst .cpp,.o,$(SRCSDBG))

PROGS = geemuboi geemuboi-dbg

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

clean:
	rm -f src/*/*.o $(PROGS)
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
