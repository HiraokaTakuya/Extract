COMPILER = g++
CFLAGS   = -std=c++11 -fno-exceptions -fno-rtti -Wextra -Ofast -MMD
LDFLAGS  =
LIBS     =
INCLUDE  =
TARGET   = extract
ifeq ($(OS),Windows_NT)
  LDFLAGS += -static
endif
OBJDIR   = obj
SOURCES  = main.cpp
OBJECTS  = $(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))
DEPENDS  = $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS) $(CFLAGS)

$(OBJDIR)/%.o: %.cpp
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

release:
	$(MAKE) CFLAGS='$(CFLAGS) -DNDEBUG' LDFLAGS='$(LDFLAGS) -flto' $(TARGET)

clean:
	rm -f $(OBJECTS) $(DEPENDS) $(TARGET) ${OBJECTS:.o=.gcda}

-include $(DEPENDS)
