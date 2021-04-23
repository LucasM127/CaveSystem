SFML=/home/lucas/Headers/SFML-2.5.1
LIBS=-lsfml-graphics -lsfml-window -lsfml-system
GCC=g++ -g -Wall

planarTest: PlanarCircles.o
	$(GCC) PlanarCircles.o -o $@ -L $(SFML)/lib $(LIBS) -Wl,-rpath=$(SFML)/lib

PlanarCircles.o: PlanarCircles.cpp
	$(GCC) $< -c -o $@ -I $(SFML)/include
	