CFLAGS = -std=c++17 -I. -I/usr/include/vulkanSDK/x86_64/include -I/usr/include/tiny_obj_loader 
LDFLAGS = -L/usr/include/vulkanSDK/x86_64/lib `pkg-config --static --libs glfw3` -lvulkan

TARGET = a.out
.SILENT:
a.out: *.hpp *.cpp
	echo "Please, be patient. \n"
	chmod +x compile.sh
	./compile.sh
	g++ $(CFLAGS) -o $(TARGET) *.cpp $(LDFLAGS) 
	./$(TARGET)
	rm -rf $(TARGET)
	echo "\n-----------------------\n         done!\n-----------------------"

