default:
	g++ -std=c++20 *.cpp -o prog Components/*.cpp -lsfml-window -lsfml-graphics -lsfml-system
	./prog
