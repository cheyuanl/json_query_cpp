CXX = g++
CPPFLAGS = -std=c++11 -Wconversion -Wall -Wextra -pedantic -Wunused -O3 -ljsoncpp
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

main: $(OBJS)
	$(CXX) $(CPPFLAGS) -o $@ $^ 

test:
	@echo testing case 1:
	bash -c "diff <(./main < test/input001.txt) <(cat test/output001.txt)"
	
	@echo testing case 2:
	bash -c "diff <(./main < test/input002.txt) <(cat test/output002.txt)"

clean: 
	rm $(OBJS) main

.PHONY: test