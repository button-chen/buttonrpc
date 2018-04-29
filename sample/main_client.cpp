#include <string>
#include <iostream>
#include <ctime>
#include "buttonrpc.hpp"

#include <Windows.h>  // use sleep

struct structdata
{
	int age;
	std::string name;
	float height;

	// must implement
	friend Serializer& operator >> (Serializer& in, structdata& d) {
		in >> d.age >> d.name >> d.height;
		return in;
	}
	friend Serializer& operator << (Serializer& out, structdata d) {
		out << d.age << d.name << d.height;
		return out;
	}
};


int main()
{
	buttonrpc client;
	client.as_client("127.0.0.1", 5555);

	while (1){
		
		int a = client.call<int>("foo", 2, 3);
		std::string b = client.call<std::string>("bar", 27, std::string("button"));
		int c = client.call<int>("test", 27, "fuckyou", 90);

		std::cout << "foo --> " << a << ", bar: " << b << ", test: " << c << std::endl;

		structdata d = {27, "button", 280.9};
		d = client.call<structdata>("structtest", d, 100);

		std::cout << "structtest: " << d.age << "-->" << d.name << "-->" << d.height << std::endl;

		Sleep(2000);
	}
	system("pause");
	return 0;
}
