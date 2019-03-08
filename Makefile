CC      = g++
CFLAGS  = -g -pthread -O0 -std=c++11 -Wall


OBJECTS = DataHandler.o Geometry_Checker.o fuzzy_c_means.o EnergyParser.o GammaScraper.o DoubleStore.o GammaTracker.o Merger.o Uncertainties.o D0_Handler.o D12_Handler.o MC_Sampler.o ComptonHandler.o PsiMerger.o

all: Program

%.o: %.cpp
	$(CC) -c $< $(CFLAGS)
	
Program: $(OBJECTS) Main.o
	$(CC) $(OBJECTS) Main.o -pthread -o compY

clean: 
	rm -f *.o $(PROGRAM)
	rm -f compY
