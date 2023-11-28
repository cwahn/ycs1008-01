#include <iostream>

#include "rt_log.hpp"

#define ASIO_STANDALONE // Not using boost but c++ standard libs

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

using namespace efp;

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

// This message handler will be invoked once for each incoming message. It
// prints the message and then sends a copy of the message back to the server.
void on_message(client *c, websocketpp::connection_hdl hdl, message_ptr msg)
{
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;
}

int main(int argc, char *argv[])
{
    // Create a client endpoint
    client c;

    c.clear_access_channels(websocketpp::log::alevel::all);
    c.clear_error_channels(websocketpp::log::elevel::all);

    std::string uri = "ws://localhost:9002";
    if (argc == 2)
        uri = argv[1];

    // Set logging to be pretty verbose (everything except message payloads)
    c.set_access_channels(websocketpp::log::alevel::all);
    c.clear_access_channels(websocketpp::log::alevel::frame_payload);

    // Initialize ASIO
    c.init_asio();

    // Register our message handler
    c.set_message_handler(bind(&on_message, &c, ::_1, ::_2));

    websocketpp::lib::error_code ec;
    client::connection_ptr con = c.get_connection(uri, ec);

    if (ec)
    {
        std::cout << "could not create connection because: " << ec.message() << std::endl;
        return 0;
    }

    c.connect(con);
    std::thread th([&]()
                   {
                       info("Spawning input taking thread");
                       while (true)
                       {
                           std::string line;
                        //    std::cout << "> ";
                           std::getline(std::cin, line);

                           debug("Sending message");

                        //    websocketpp::lib::error_code ec;
                            con->send(line);

                           if (ec)
                               std::cout << "Echo failed because: " << ec.message() << std::endl;
                       }

                       warn("Exiting input thread"); });
    c.run();
}
//    c.send(hdl, line, msg->get_opcode(), ec);