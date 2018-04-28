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
	R call(std::string name, P1 p1);

	template<typename R, typename P1, typename P2>
	R call(std::string name, P1 p1, P2 p2);

	template<typename R, typename P1, typename P2, typename P3>
	R call(std::string name, P1 p1, P2 p2, P3 p3);

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

	// PROXY CLASS MEMBER
	template<typename R, typename C, typename S>
	void callproxy_(R(C::*)(), S* s, Serializer* pr, const char* data, int len) {
		
	}

	template<typename R, typename C, typename S, typename P1>
	void callproxy_(R(C::*)(P1), S* s, Serializer* pr, const char* data, int len);

	template<typename R, typename C, typename S, typename P1, typename P2>
	void callproxy_(R(C::* func)(P1, P2), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2)>(std::bind(func, s, std::placeholders::_1, std::placeholders::_2)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2, typename P3>
	void callproxy_(R(C::*)(P1, P2, P3), S* s, Serializer* pr, const char* data, int len);

	// PORXY FUNCTIONAL
	template<typename R>
	void callproxy_(std::function<R()>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1>
	void callproxy_(std::function<R(P1)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2>
	void callproxy_(std::function<R(P1, P2)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2, typename P3>
	void callproxy_(std::function<R(P1, P2, P3)>, Serializer* pr, const char* data, int len);

private:
	std::map<std::string, std::function<void(Serializer*, const char*, int)>> m_handlers;

	zmq::context_t m_context;
	zmq::socket_t* m_socket;

	int m_role;
};

buttonrpc::buttonrpc() : m_context(1){ }

buttonrpc::~buttonrpc(){ }

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
		Serializer ds(iodev, Serializer::LittleEndian);
		uint8 namelen;
		ds >> namelen;
		const char* p = ds.current();

		std::string funname(p, p+namelen);
		Serializer* r = call_(funname, p+(int)namelen, ds.size()-namelen);

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
	Serializer* ds = new Serializer(StreamBuffer(), Serializer::LittleEndian);
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
	Serializer ds(StreamBuffer(data, len), Serializer::LittleEndian);
	R r = func();
	(*pr) << r;
}

template<typename R, typename P1>
void buttonrpc::callproxy_(std::function<R(P1)> func, Serializer* pr, const char* data, int len)
{
	Serializer ds(StreamBuffer(data, len), Serializer::LittleEndian);
	P1 p1;
	ds >> p1;
	R r = func(p1);
	(*pr) << r;
}

template<typename R, typename P1, typename P2>
void buttonrpc::callproxy_(std::function<R(P1, P2)> func, Serializer* pr, const char* data, int len )
{
	Serializer ds(StreamBuffer(data, len), Serializer::LittleEndian);
	P1 p1; P2 p2;
	ds >> p1 >> p2;
	R r = func(p1, p2);
	(*pr) << r;
}

template<typename R, typename P1, typename P2, typename P3>
void buttonrpc::callproxy_(std::function<R(P1, P2, P3)> func, Serializer* pr, const char* data, int len)
{
	Serializer ds(StreamBuffer(data, len), Serializer::LittleEndian);
	P1 p1; P2 p2; P3 p3;
	ds >> p1 >> p2 >> p3;
	R r = func(p1, p2, p3);
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
	Serializer ds(StreamBuffer(), Serializer::LittleEndian);
	ds << (uint8)name.size();
	ds.write_raw_data((char*)name.c_str(), name.size());

	return net_call<R>(ds);
}

template<typename R, typename P1>
R buttonrpc::call(std::string name, P1 p1)
{
	Serializer ds(StreamBuffer(), Serializer::LittleEndian);
	ds << (uint8)name.size();
	ds.write_raw_data((char*)name.c_str(), name.size());
	ds << p1;

	return net_call<R>(ds);
}

template<typename R, typename P1, typename P2>
R buttonrpc::call( std::string name, P1 p1, P2 p2 )
{
	Serializer ds(StreamBuffer(), Serializer::LittleEndian);
	ds << (uint8)name.size();
	ds.write_raw_data((char*)name.c_str(), name.size());
	ds << p1;
	ds << p2;

	return net_call<R>(ds);
}

template<typename R, typename P1, typename P2, typename P3>
R buttonrpc::call(std::string name, P1 p1, P2 p2, P3 p3)
{
	Serializer ds(StreamBuffer(), Serializer::LittleEndian);
	ds << (uint8)name.size();
	ds.write_raw_data((char*)name.c_str(), name.size());
	ds << p1;
	ds << p2;
	ds << p3;

	return net_call<R>(ds);
}
