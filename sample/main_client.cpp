#include <string>
#include <iostream>
#include <ctime>
#include "buttonrpc.hpp"

#include <Windows.h>

int main()
{
	buttonrpc client;
	client.as_client("127.0.0.1", 5555);

	time_t bb = time(0);
	int count = 0;
	while (1){
		
		int a = client.call<int>("foo", 2, 3);
		std::string b = client.call<std::string>("bar", 27, std::string("button"));
		int c = client.call<int>("test", 27, std::string("test"));

		std::cout << "foo --> " << a << ", bar: " << b << ", test: " << c << std::endl;
		count++;
		if (time(0) - bb >= 10){
			break;
		}
	}
	std::cout << "10s call count: " << count << std::endl;
	system("pause");
	return 0;
}
