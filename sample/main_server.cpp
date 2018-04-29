#include <string>
#include <iostream>
#include "buttonrpc.hpp"

int foo(int age, int mm){
	return age + mm;
}

std::string bar(int age, std::string name){
	return (name + " is handsome");
}

class test
{
public:
	int foo(int age, std::string ff, int gg) {
		std::cout << age << "--" << ff << "--" << gg << std::endl;
		return gg;
	}
};

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

structdata teststructparam(structdata d, int weigth)
{
	structdata ret;
	ret.age = d.age + 100;
	ret.name = d.name + "kk";
	ret.height = d.height + 8;
	return ret;
}

int main()
{

	buttonrpc server;
	server.as_server(5555);

	test tt;
	server.bind("test", &test::foo, &tt);
	server.bind("foo", foo);
	server.bind("bar", std::function<std::string(int, std::string)>(bar));
	server.bind("structtest", teststructparam);

	server.run();

	return 0;
}
