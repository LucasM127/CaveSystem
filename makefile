SFML=/home/lucas/Headers/SFML-2.5.1
LIBS=-lsfml-graphics -lsfml-window -lsfml-system
GCC=g++ -g -Wall

borderTest: MiterOutline.o Kruskal.o NodeGenerator.o OutlineGraph.o
	$(GCC) MiterOutline.o Kruskal.o NodeGenerator.o OutlineGraph.o -o $@ -L $(SFML)/lib $(LIBS) -Wl,-rpath=$(SFML)/lib

mstTest: KruskalTest.o Kruskal.o NodeGenerator.o
	$(GCC) KruskalTest.o Kruskal.o NodeGenerator.o -o $@ -L $(SFML)/lib $(LIBS) -lGL -Wl,-rpath=$(SFML)/lib

outlineTest: OutlineTest.o OutlineGraph.o NodeGenerator.o Kruskal.o
	$(GCC) OutlineTest.o OutlineGraph.o NodeGenerator.o Kruskal.o -o $@ -L $(SFML)/lib $(LIBS) -Wl,-rpath=$(SFML)/lib

genNodeTest: NodeGenerator.o NodeGenTest.o
	$(GCC) NodeGenerator.o NodeGenTest.o -o $@ -L $(SFML)/lib $(LIBS) -Wl,-rpath=$(SFML)/lib

baryTest: BarycentricTriangle.o
	$(GCC) BarycentricTriangle.o -o $@ -L $(SFML)/lib $(LIBS) -Wl,-rpath=$(SFML)/lib

tangentTest: Tangent2Circles.cpp
	$(GCC) $< -o $@ -I $(SFML)/include -L $(SFML)/lib $(LIBS) -lGL -Wl,-rpath=$(SFML)/lib

randomPointsTest: RandomPoints.cpp
	$(GCC) $< -o $@ -I $(SFML)/include -L $(SFML)/lib $(LIBS) -lGL -Wl,-rpath=$(SFML)/lib

BarycentricTriangle.o: BarycentricTriangle.cpp
	$(GCC) $< -c -o $@ -I $(SFML)/include

OutlineTest.o: OutlineTest.cpp
	$(GCC) $< -c -o $@ -I $(SFML)/include

OutlineGraph.o: OutlineGraph.cpp OutlineGraph.hpp
	$(GCC) $< -c -o $@ -I $(SFML)/include

MiterOutline.o: MiterOutline.cpp
	$(GCC) $< -c -o $@ -I $(SFML)/include
	
main.o: main.cpp
	$(GCC) $< -c -o $@ -I $(SFML)/include

KruskalTest.o: KruskalTest.cpp Kruskal.o NodeGenTest.o
	$(GCC) $< -c -o $@ -I $(SFML)/include

Kruskal.o: Kruskal.cpp Kruskal.hpp
	$(GCC) $< -c -o $@ -I $(SFML)/include

NodeGenTest.o: NodeGenTest.cpp
	$(GCC) $< -c -o $@ -I $(SFML)/include

NodeGenerator.o: NodeGenerator.cpp NodeGenerator.hpp
	$(GCC) $< -c -o $@ -I $(SFML)/include