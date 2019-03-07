// std
#include <iostream>

// boost
#include <boost/smart_ptr.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/move/move.hpp>
#include <boost/bind.hpp>

/**
 * @class        server
 * @brief        server
 * @author       wuzhuobin
 * @date         Mar.07.2019
 * 
 */
class server: public boost::enable_shared_from_this<server>
{
  public: 
    server(boost::asio::io_context &io_context, boost::asio::ip::tcp::endpoint endpoint):
      acceptor(io_context), 
      socket(io_context)
    {
        boost::system::error_code error_code;

        // Open the acceptor
        this->acceptor.open(endpoint.protocol(), error_code);
        if(error_code)
        {
            std::cerr << "Failed open acceptor. \n";
            std::cerr << error_code << "\n";
            return;
        }

        // Allow address reuse
        this->acceptor.set_option(boost::asio::socket_base::reuse_address(true));
        if(error_code)
        {
            // fail(error_code, "set_option");
            std::cerr << error_code << "\n";
            return;
        }

        // Bind to the server address
        this->acceptor.bind(endpoint, error_code);
        if(error_code)
        {
            // fail(error_code, "bind");
            std::cerr << error_code << "\n";
            return;
        }

        // Start listening for connections
        this->acceptor.listen(
            boost::asio::socket_base::max_listen_connections, error_code);
        if(error_code)
        {
            // fail(error_code, "listen");
            std::cerr << error_code << "\n";
            return;
        }

        // Start accepting incoming connection.
        // this->acceptor.async_accept(this->socket, 
        //   boost::bind(&server::accepted, this->shared_from_this(), _1));
    }
    void run()
    {
        this->acceptor.async_accept(this->socket, 
          boost::bind(&server::accepted, this->shared_from_this(), _1));
    }
  private:
    void accepted(boost::system::error_code error_code)
    {
      if(error_code)
      {
        std::cerr << error_code << '\n';
        std::cerr << error_code.message() << '\n';
        std::cerr << error_code.value() << '\n';
      }
      else
      {
        boost::make_shared<session>(boost::move(this->socket))->run();
      }
      this->acceptor.async_accept(this->socket,
        boost::bind(&server::accepted, this->shared_from_this(), _1));      
    }
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;

    class session: public boost::enable_shared_from_this<session>
    {
    public:
      explicit session(boost::asio::ip::tcp::socket socket) :
        websocket(boost::move(socket)),
        strand(this->websocket.get_executor())
      {
      }
      void run()
      {
        this->websocket.async_accept(
          boost::asio::bind_executor(this->strand,
            boost::bind(&session::accepted, this->shared_from_this(), _1)));
      }
    private:
      void accepted(boost::system::error_code error_code)
      {
        if(error_code)
        {
          std::cerr << error_code << '\n';
          return;
        }
        this->websocket.async_read(this->multi_buffer, 
          boost::asio::bind_executor(this->strand, boost::bind(&session::readen, this->shared_from_this(), _1, _2)));
      }

      void readen(boost::system::error_code, std::size_t bytes_transferred)
      {
        // boost::ignore_unused(bytes_transferred);
        // (void)(bytes_transferred);
        std::cerr << "bytes: " << bytes_transferred << '\n';
      }

      boost::beast::websocket::stream<boost::asio::ip::tcp::socket> websocket;
      boost::asio::strand<boost::asio::io_context::executor_type> strand;
      boost::beast::multi_buffer multi_buffer;
    };
};



int main(int argc, char **argv) {
  std::cout << "Hello world!\n";
  boost::asio::io_context io_context;
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("0.0.0.0"), 8080);
  boost::make_shared<server>(io_context, endpoint)->run();
  io_context.run();

  return EXIT_SUCCESS;
}