OBJS = \
	Assertions.o \
	Test.o \
	testings.o
APP = a.out


CXX = g++
CXX_STANDARD = -std=c++17 -Wall -g
ADD_FLAGS =
DBG_FLAGS = -DTEST -DFRANKENSTEIN -DDEBUG
CXXFLAGS = --std=c++2a -Wall -Werror -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wpointer-arith -Wformat -Wreturn-type -Wsign-compare -Wmultichar -Wformat-nonliteral -Winit-self -Wuninitialized -Wno-deprecated -Wformat-security -pipe -g $(ADD_FLAGS)
LD = $(CXX)
LDFLAGS = -lm

all: $(APP)

rundbg:
	make ADD_FLAGS="$(DBG_FLAGS)"
	@./$(APP)
run:
	@make
	@./$(APP)

$(APP): $(OBJS)
	$(LD) -o $(APP) $(LDFLAGS) $(OBJS)
	@rm $(OBJS)
.cpp.o:
	@echo "CXX $<"; $(CXX) $(CXXFLAGS) -o $@ -c $<

-include $(OBJS:%.o=%.d)

clean:
	@echo "CLEAN"; rm -f $(APP) $(OBJS) *.d
