/**
*
*	buttonrpc library
*	Copyright 2018-04-28 Button
*
*/

#pragma once
#include <string>
#include <map>
#include <string>
#include <sstream>
#include <functional>
#include <zmq.hpp>
#include "Serializer.hpp"

class buttonrpc
{
public:
	enum rpc_role{
		RPC_CLIENT,
		RPC_SERVER
	};
	buttonrpc();
	~buttonrpc();

	// network
	void as_client(std::string ip, int port);
	void as_server(int port);
	void send(zmq::message_t& data);
	void recv(zmq::message_t& data);
	void run();

public:
	// server
	template<typename F>
	void bind(std::string name, F func);

	template<typename F, typename S>
	void bind(std::string name, F func, S* s);

	// client
	template<typename R>
	R call(std::string name);

	template<typename R, typename P1>
	R call(std::string name, P1);

	template<typename R, typename P1, typename P2>
	R call(std::string name, P1, P2);

	template<typename R, typename P1, typename P2, typename P3>
	R call(std::string name, P1, P2, P3);

	template<typename R, typename P1, typename P2, typename P3, typename P4>
	R call(std::string name, P1, P2, P3, P4);

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
	R call(std::string name, P1, P2, P3, P4, P5);

private:
	Serializer* call_(std::string name, const char* data, int len);

	template<typename R>
	R net_call(Serializer& ds);

	template<typename F>
	void callproxy(F fun, Serializer* pr, const char* data, int len);

	template<typename F, typename S>
	void callproxy(F fun, S* s, Serializer* pr, const char* data, int len);

	// PROXY FUNCTION POINT
	template<typename R>
	void callproxy_(R(*func)(), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R()>(func), pr, data, len);
	}

	template<typename R, typename P1>
	void callproxy_(R(*func)(P1), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1)>(func), pr, data, len);
	}

	template<typename R, typename P1, typename P2>
	void callproxy_(R(*func)(P1, P2), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2)>(func), pr, data, len);
	}

	template<typename R, typename P1, typename P2, typename P3>
	void callproxy_(R(*func)(P1, P2, P3), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3)>(func), pr, data, len);
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4>
	void callproxy_(R(*func)(P1, P2, P3, P4), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3, P4)>(func), pr, data, len);
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
	void callproxy_(R(*func)(P1, P2, P3, P4, P5), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3, P4, P5)>(func), pr, data, len);
	}

	// PROXY CLASS MEMBER
	template<typename R, typename C, typename S>
	void callproxy_(R(C::* func)(), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R()>(std::bind(func, s)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1>
	void callproxy_(R(C::* func)(P1), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1)>(std::bind(func, s, std::placeholders::_1)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2>
	void callproxy_(R(C::* func)(P1, P2), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2)>(std::bind(func, s, std::placeholders::_1, std::placeholders::_2)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2, typename P3>
	void callproxy_(R(C::* func)(P1, P2, P3), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3)>(std::bind(func, s, 
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2, typename P3, typename P4>
	void callproxy_(R(C::* func)(P1, P2, P3, P4), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3, P4)>(std::bind(func, s,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2, typename P3, typename P4, typename P5>
	void callproxy_(R(C::* func)(P1, P2, P3, P4, P5), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3, P4, P5)>(std::bind(func, s,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)), pr, data, len);
	}

	// PORXY FUNCTIONAL
	template<typename R>
	void callproxy_(std::function<R()>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1>
	void callproxy_(std::function<R(P1)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2>
	void callproxy_(std::function<R(P1, P2)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2, typename P3>
	void callproxy_(std::function<R(P1, P2, P3)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2, typename P3, typename P4>
	void callproxy_(std::function<R(P1, P2, P3, P4)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
	void callproxy_(std::function<R(P1, P2, P3, P4, P5)>, Serializer* pr, const char* data, int len);

private:
	std::map<std::string, std::function<void(Serializer*, const char*, int)>> m_handlers;

	zmq::context_t m_context;
	zmq::socket_t* m_socket;

	int m_role;
};

buttonrpc::buttonrpc() : m_context(1){ }

buttonrpc::~buttonrpc(){ 
	m_socket->close();
	delete m_socket;
	m_context.close();
}

// network
void buttonrpc::as_client( std::string ip, int port )
{
	m_role = RPC_CLIENT;
	m_socket = new zmq::socket_t(m_context, ZMQ_REQ);
	ostringstream os;
	os << "tcp://" << ip << ":" << port;
	m_socket->connect (os.str());
}

void buttonrpc::as_server( int port )
{
	m_role = RPC_SERVER;
	m_socket = new zmq::socket_t(m_context, ZMQ_REP);
	ostringstream os;
	os << "tcp://*:" << port;
	m_socket->bind (os.str());
}

void buttonrpc::send( zmq::message_t& data )
{
	m_socket->send(data);
}

void buttonrpc::recv( zmq::message_t& data )
{
	m_socket->recv(&data);
}

void buttonrpc::run()
{
	while (1){
		zmq::message_t data;
		recv(data);
		StreamBuffer iodev((char*)data.data(), data.size());
		Serializer ds(iodev);

		std::string funname;
		ds >> funname;
		Serializer* r = call_(funname, ds.current(), ds.size()- funname.size());

		zmq::message_t retmsg (r->size());
		memcpy (retmsg.data (), r->data(), r->size());
		send(retmsg);
		delete r;
	}
}

// 处理函数相关

Serializer* buttonrpc::call_(std::string name, const char* data, int len)
{
	auto fun = m_handlers[name];
	Serializer* ds = new Serializer();
	fun(ds, data, len);
	ds->reset();
	return ds;
}

template<typename F>
void buttonrpc::bind( std::string name, F func )
{
	m_handlers[name] = std::bind(&buttonrpc::callproxy<F>, this, func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

template<typename F, typename S>
inline void buttonrpc::bind(std::string name, F func, S* s)
{
	m_handlers[name] = std::bind(&buttonrpc::callproxy<F, S>, this, func, s, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

template<typename F>
void buttonrpc::callproxy( F fun, Serializer* pr, const char* data, int len )
{
	callproxy_(fun, pr, data, len);
}

template<typename F, typename S>
inline void buttonrpc::callproxy(F fun, S * s, Serializer * pr, const char * data, int len)
{
	callproxy_(fun, s, pr, data, len);
}

template<typename R>
void buttonrpc::callproxy_(std::function<R()> func, Serializer* pr, const char* data, int len)
{
	Serializer ds(StreamBuffer(data, len));
	R r = func();
	(*pr) << r;
}

template<typename R, typename P1>
void buttonrpc::callproxy_(std::function<R(P1)> func, Serializer* pr, const char* data, int len)
{
	Serializer ds(StreamBuffer(data, len));
	P1 p1;
	ds >> p1;
	R r = func(p1);
	(*pr) << r;
}

template<typename R, typename P1, typename P2>
void buttonrpc::callproxy_(std::function<R(P1, P2)> func, Serializer* pr, const char* data, int len )
{
	Serializer ds(StreamBuffer(data, len));
	P1 p1; P2 p2;
	ds >> p1 >> p2;
	R r = func(p1, p2);
	(*pr) << r;
}

template<typename R, typename P1, typename P2, typename P3>
void buttonrpc::callproxy_(std::function<R(P1, P2, P3)> func, Serializer* pr, const char* data, int len)
{
	Serializer ds(StreamBuffer(data, len));
	P1 p1; P2 p2; P3 p3;
	ds >> p1 >> p2 >> p3;
	R r = func(p1, p2, p3);
	(*pr) << r;
}

template<typename R, typename P1, typename P2, typename P3, typename P4>
void buttonrpc::callproxy_(std::function<R(P1, P2, P3, P4)> func, Serializer* pr, const char* data, int len)
{
	Serializer ds(StreamBuffer(data, len));
	P1 p1; P2 p2; P3 p3; P4 p4;
	ds >> p1 >> p2 >> p3 >> p4;
	R r = func(p1, p2, p3, p4);
	(*pr) << r;
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
void buttonrpc::callproxy_(std::function<R(P1, P2, P3, P4, P5)> func, Serializer* pr, const char* data, int len)
{
	Serializer ds(StreamBuffer(data, len));
	P1 p1; P2 p2; P3 p3; P4 p4; P5 p5;
	ds >> p1 >> p2 >> p3 >> p4 >> p5;
	R r = func(p1, p2, p3, p4, p5);
	(*pr) << r;
}

template<typename R>
inline R buttonrpc::net_call(Serializer& ds)
{
	zmq::message_t request(ds.size() + 1);
	memcpy(request.data(), ds.data(), ds.size());
	send(request);

	zmq::message_t reply;
	recv(reply);
	ds.clear();
	ds.write_raw_data((char*)reply.data(), reply.size());
	ds.reset();
	R r;
	ds >> r;
	return r;
}

template<typename R>
R buttonrpc::call(std::string name)
{
	Serializer ds;
	ds << name;
	return net_call<R>(ds);
}

template<typename R, typename P1>
R buttonrpc::call(std::string name, P1 p1)
{
	Serializer ds;
	ds << name << p1;
	return net_call<R>(ds);
}

template<typename R, typename P1, typename P2>
R buttonrpc::call( std::string name, P1 p1, P2 p2 )
{
	Serializer ds;
	ds << name << p1 << p2;
	return net_call<R>(ds);
}

template<typename R, typename P1, typename P2, typename P3>
R buttonrpc::call(std::string name, P1 p1, P2 p2, P3 p3)
{
	Serializer ds;
	ds << name << p1 << p2 << p3;
	return net_call<R>(ds);
}

template<typename R, typename P1, typename P2, typename P3, typename P4>
inline R buttonrpc::call(std::string name, P1 p1, P2 p2, P3 p3, P4 p4)
{
	Serializer ds;
	ds << name << p1 << p2 << p3 << p4;
	return net_call<R>(ds);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
inline R buttonrpc::call(std::string name, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
{
	Serializer ds;
	ds << name << p1 << p2 << p3 << p4 << p5;
	return net_call<R>(ds);
}
