#include "pch.h"
#include "WebListener.h"

int main()
{
	WSAInitializer wsa; //initialize WSA for socket usage

	try {
		WebListener listener;
		
	}
	catch (const std::exception& e) {
		std::cerr << "[Exception] " << e.what() << std::endl;
	}

	system("pause");
	return 0;
}
