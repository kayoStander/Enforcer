CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

output: *.cpp *.hpp *.h
	chmod +x compile.sh
	./compile.sh
	g++ $(CFLAGS) -o output *.cpp $(LDFLAGS) && ./output && rm ./output
