#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "logger.hpp"

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

#include <termios.h>
#include <unistd.h>

char getch()
{
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}

int main(int argc, char *argv[])
{
    std::string uri = "ws://localhost:9002";

    if (argc == 2)
        uri = argv[1];

    // Create a client endpoint
    client c;

    // Initialize ASIO
    c.init_asio();

    // c.clear_access_channels(websocketpp::log::alevel::none);
    c.clear_error_channels(websocketpp::log::elevel::none);

    // // Set logging to be pretty verbose (everything except message payloads)
    // c.set_access_channels(websocketpp::log::alevel::all);
    c.clear_access_channels(websocketpp::log::alevel::frame_payload);
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

    int position = 0;
    std::thread th([&]()
                   {
        info("Spawning input taking thread");
        while (true)
        {
            //    std::string line;
            // //    std::cout << "> ";
            //    std::getline(std::cin, line);

            //    debug("Sending message");

            // //    websocketpp::lib::error_code ec;
            //     con->send(line);

            switch (getch())
            {
            case 65:
                con->send("p" + std::to_string(++position));
                break;
            case 66:
                con->send("p" + std::to_string(--position));
                break;
            }
            std::cout << "Sent position: " << position << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(1000/60));

        }

            warn("Exiting input thread"); });
    c.run();
}
//    c.send(hdl, line, msg->get_opcode(), ec);