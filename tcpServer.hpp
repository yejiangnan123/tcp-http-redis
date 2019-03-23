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
/*
-------------------------------------------
异步接收连接；
异步接收数据；
开启多个线程同时处理数据；防止同一个客户端同时在多个线程接收数据，所以这里是系统接收数据后再开启异步接收；
接收到的数据调用外部函数进行组包，正确数据包需要放入队列；这里组包函数进行打印即可；也可以调用回显，发送接收到的数据；
-------------------------------------------
*/
#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class session
  : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket)
    : socket_(std::move(socket))
  {
  }

  void start()
  {
    do_read();
  }

private:
  void do_read()
  {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            do_write(length);
          }
        });
  }

  void do_write(std::size_t length)
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            do_read();
          }
        });
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class server
{
public:
  server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<session>(std::move(socket))->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
};

//-------------------------------------------------
  class Test{
    public:
      void testListener(int port=6688) {
        try
        {
          boost::asio::io_context io_context;

          server s(io_context, port);

          io_context.run();
        }
        catch (std::exception& e)
        {
          std::cerr << "Exception: " << e.what() << "\n";
        }
      }
  };
#endif
