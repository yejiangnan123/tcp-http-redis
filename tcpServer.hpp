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
#include <iostream>
#include <assert.h>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/ptr_container/ptr_list.hpp> 
#include <boost/asio/io_context.hpp>
namespace mytcp{ 
  using namespace boost::asio;
  using namespace boost;
  using namespace std;
  using tcp = boost::asio::ip::tcp;
  typedef std::shared_ptr<boost::asio::io_context> io_context_ptr;
//---------------------------------
  class listener : public std::enable_shared_from_this<listener> {
     private:
        boost::asio::io_context  _ioc;
        tcp::acceptor         _acceptor(_ioc);
        tcp::socket           _socket(_ioc);
        ptr_list<io_context_ptr>   _ioc_list;
        int thread_num = 4;
     public:
        listener(tcp::endpoint &endpoint) {
           boost::system::error_code ec;

           _acceptor.open( endpoint.protocol(), ec );
           if( ec ) { on_fail( ec, "open" ); return; }

           _acceptor.set_option( boost::asio::socket_base::reuse_address(true) );

           _acceptor.bind( endpoint, ec );
           if( ec ) { on_fail( ec, "bind" ); return; }

           _acceptor.listen( boost::asio::socket_base::max_listen_connections, ec );
           if( ec ) on_fail( ec, "listen" );
        }

        void run() {
           assert(_acceptor.is_open());
           do_accept();
        }

        void do_accept() {
           _acceptor.async_accept( _socket, [self=shared_from_this()]( boost::system::error_code ec ){ self->on_accept(ec); } );
        }

        void on_fail( boost::system::error_code ec, const char* what ) {
           cout<<string(ec.message())<<" "<<string(what)<<"\n";
        }

        void on_accept( boost::system::error_code ec ){
           if( ec ) {
              if( ec == boost::system::errc::too_many_files_open )
                 do_accept();
              return;
           }
           std::shared_ptr<session> newsession;
           try {
              newsession = std::make_shared<session>(_socket);
              std::shared_ptr<int> newint = std::make_shared<int>();
           }
           catch( std::exception& e ) {
              _socket.close();
           }
           if( newsession ) {
              newsession->run();
           }
           do_accept();
       }
  };
//client for tcp------------------------------
  class client{
  };
//-------------------------------------------------
    class Test{
      public:
        Test(){}
        listener*                                    _listener;
        tcp::endpoint endpoint;
        void testListener() {
          //endpoint = ip::tcp::endpoint(ip::address::from_string("127.0.0.1"),6688);
          _listener = new listener(_ioc,endpoint);
          _listener->run();
        }
    };
}
#endif
