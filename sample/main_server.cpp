#include <string>
#include <iostream>
#include "buttonrpc.hpp"

int foo(int age, int mm){
	return age + mm;
}

std::string bar(int age, std::string name){
	return (name+" is handsome");
}

class test
{
public:
	int foo(int age, std::string name) {
		return 100;
	}
};

int main()
{
	buttonrpc server;
	server.as_server(5555);

	test tt;
	server.bind("test", &test::foo, &tt);
	server.bind("foo", foo);
	server.bind("bar", std::function<std::string(int, std::string)>(bar));

	server.run();

	return 0;
}
