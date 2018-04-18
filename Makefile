TRG:=g711conv
SRC:=g711.c wav.cpp main.cpp
OBJ:=$(SRC:.c=.o)
OBJ:=$(OBJ:.cpp=.o)

all: $(TRG)

$(TRG): $(OBJ)
	g++ -std=c++11 $(OBJ) -o $(TRG)

clean:
	rm -f $(TRG)
	rm -f $(OBJ) main_old.o

%.o: %.c
	gcc -std=c99 -c $< -o $@

%.o: %.cpp
	g++ -std=c++11 -c $< -o $@

old: g711.o main_old.o
	g++ -std=c++11 main_old.o g711.o -o g711conv
