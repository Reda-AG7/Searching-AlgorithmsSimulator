default:
	g++ -std=c++20 -o prog *.cpp Components/*.cpp -lsfml-window -lsfml-graphics -lsfml-system
	
	./prog
