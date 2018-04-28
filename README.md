
# buttonrpc - a simple rpc framework for C++

## Blueprint

- Construct a simple generic network library
- Support simple remote procedure call (RPC) as call a local procedure
- Simple and easy to use and manage

## Example
server:

```c++
#include "buttonrpc.hpp"

int foo(int age, int mm){
	return age + mm;
}

int main()
{
	buttonrpc server;
	server.as_server(5555);

	server.bind("foo", foo);
	server.run();

	return 0;
}
```

client: 

```c++
#include <iostream>
#include "buttonrpc.hpp"

int main()
{
	buttonrpc client;
	client.as_client("127.0.0.1", 5555);
	int a = client.call<int>("foo", 2, 3);
	std::cout << "call foo result: " << a << std::endl;
	system("pause");
	return 0;
}

// output: call foo result: 5

```

## Dependences
- [zeromq](http://zguide.zeromq.org/page:all)


## Building
vs2010 or late 

## Usage

See also example/

