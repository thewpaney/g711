TRG=g711conv
all: $(TRG)

$(TRG):
	g++ g711.c main.cpp -o $(TRG)

clean:
	rm -f $(TRG)
