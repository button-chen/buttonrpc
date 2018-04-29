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
		
		int a = client.call<int>("foo", 2, 3).val();
		buttonrpc::value_t<std::string> b = client.call<std::string>("bar", 27, std::string("button"));
		buttonrpc::value_t<int> c = client.call<int>("test", 27, "fuckyou", 90);

		std::cout << "foo --> " << a << ", bar: " << b.val() << ", test: " << c.val() << std::endl;

		structdata  dd = {27, "button", 280.9};
		buttonrpc::value_t<structdata> d = client.call<structdata>("structtest", dd, 100);

		std::cout << "structtest: " << d.val().age << "-->" << d.val().name << "-->" << d.val().height << std::endl;

		buttonrpc::value_t<void> xx = client.call<void>("funxx1", 9);
		if (!xx.valid()) {
			std::cout << xx.error_msg() << std::endl;
		}

		Sleep(1000);
	}
	system("pause");
	return 0;
}
