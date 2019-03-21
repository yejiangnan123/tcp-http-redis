/*
------------------------------------------
这是一个基于boost::asio异步tcp服务器；
可以设置接收线程数量，默认为4；
每个客户端有自己的数据缓存区，缓冲区大小可动态调整；
这个类只有生产线程；这里会写一个用于测试的消费线程，功能是回显，即把接收到的数据返回给用户；
这里会有一个组包类，用于拆分数据包，但是测试需要客户端配合；
这里写一个客户端类，用于配合测试。
------------------------------------------
*/
#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind/bind.hpp>
namespace tcp{ 
  using namespace boost::asio;
  using namespace std;
//server for tcp-----------------------------
  class server
  {
    typedef server this_type;
    typedef ip::tcp::acceptor acceptor_type;
    typedef ip::tcp::endpoint endpoint_type;
    typedef ip::tcp::socket socket_type;
    typedef ip::address address_type;
    typedef boost::shared_ptr<socket_type> sock_ptr;
  private:
    io_service m_io;
    acceptor_type m_acceptor;
  public:
    server() : m_acceptor(m_io, endpoint_type(ip::tcp::v4(), 6688)) { accept(); }
    void run()  { m_io.run(); }
    void accept(){
      sock_ptr sock(new socket_type(m_io));
      m_acceptor.async_accept(*sock, boost::bind(&this_type::accept_handler, this, boost::asio::placeholders::error, sock));
    }
    void accept_handler(const boost::system::error_code& ec, sock_ptr sock){
      if (ec) { return; }
      cout<<"Client:";
      cout<<sock->remote_endpoint().address()<<endl;
      sock->async_write_some(buffer("hello asio"), boost::bind(&this_type::write_handler, this, boost::asio::placeholders::error));
      // 发送完毕后继续监听，否则io_service将认为没有事件处理而结束运行
      accept();
    }
    void write_handler(const boost::system::error_code&ec){
      cout<<"send msg complete"<<endl;
    }
  };
//client for tcp------------------------------
  class client{
  };
}
#endif
