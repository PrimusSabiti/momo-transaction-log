FLAGS = -std=c++17 -Wall -Wextra -g
all: app tests timing
app: main.cpp *.h
	g++ $(FLAGS) -o app main.cpp
tests: tests.cpp *.h
	g++ $(FLAGS) -o tests tests.cpp
timing: timing.cpp *.h
	g++ -std=c++17 -O2 -o timing timing.cpp
asan: tests.cpp *.h
	g++ $(FLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer -o tests_asan tests.cpp
	./tests_asan
clean:
	rm -f app tests timing tests_asan
