#include "first_app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main(){
	lve::firstApp app{};

	try{
		app.run();
	}catch(const std::exception &e){
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
