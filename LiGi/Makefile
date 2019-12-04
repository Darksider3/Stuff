OBJS = \
	main.o 
APP = a.out


CXX = g++
CXX_STANDARD = -std=c++17 -Wall -g
CXXFLAGS = -DDEBUG --std=c++2a -Wall -Werror -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wpointer-arith -Wformat -Wreturn-type -Wsign-compare -Wmultichar -Wformat-nonliteral -Winit-self -Wuninitialized -Wno-deprecated -Wformat-security -pipe -g
LD = g++
LDFLAGS = -lm

all: $(APP)

run:
	@make &> /dev/null
	@./$(APP)

$(APP): $(OBJS)
	$(LD) -o $(APP) $(LDFLAGS) $(OBJS)
	@rm $(OBJS)
.cpp.o:
	@echo "CXX $<"; $(CXX) $(CXXFLAGS) -o $@ -c $<

-include $(OBJS:%.o=%.d)

clean:
	@echo "CLEAN"; rm -f $(APP) $(OBJS) *.d
