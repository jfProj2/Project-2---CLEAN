#all: 1730ed

#1730ed: tui.o io.o main.o
        #g++ -Wall -lmenu -lncurses -o 1730ed tui.o io.o main.o
#tui.o: tui.cpp tui.h
        #g++ -Wall -std=c++14 -g -O0 -pedantic-errors -c -lmenu -lncurses  tui.cpp
#io.o: io.cpp io.h
        #g++ -Wall -std=c++14 -g -O0 -pedantic-errors -c -lmenu -lncurses io.cpp

#main.o: main.cpp tui.h io.h
        #g++ -Wall -std=c++14 -g -O0 -pedantic-errors -c -lmenu -lncurses main.cpp

#clean:
        #rm -f 1730ed
        #rm -f tui.o
        #rm -f io.o
        #rm -f main.o


all: bananas2

bananas2: bananas2.o
        g++ -Wall -lmenu -lncurses -o  bananas2 bananas2.o

bananas2.o: bananas2.cpp methods2.h
        g++ -Wall -std=c++14 -g -O0 -pedantic-errors -c -lmenu -lncurses bananas2.cpp

clean:
        rm -f bananas2
        rm -f bananas2.o
