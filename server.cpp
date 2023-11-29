#include <set>

#include "logger.hpp"

#define ASIO_STANDALONE // Not using boost but c++ standard libs
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

using namespace efp;

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

class broadcast_server
{
public:
    broadcast_server()
    {
        m_server.clear_access_channels(websocketpp::log::alevel::none);
        m_server.clear_error_channels(websocketpp::log::elevel::none);

        m_server.init_asio();

        m_server.set_open_handler(bind(&broadcast_server::on_open, this, ::_1));
        m_server.set_close_handler(bind(&broadcast_server::on_close, this, ::_1));
        m_server.set_message_handler(bind(&broadcast_server::on_message, this, ::_1, ::_2));
    }

    void on_open(connection_hdl hdl)
    {
        info("Connection opened");
        m_connections.push_back(hdl);
    }

    void on_close(connection_hdl hdl)
    {
        info("Connection closed");
        const auto maybe_index = find_index([&](auto x)
                                            { return x.lock() == hdl.lock(); },
                                            m_connections);
        if (maybe_index)
            m_connections.erase(maybe_index.value());
    }

    void on_message(connection_hdl hdl, server::message_ptr msg)
    {
        info("Message received");
        const auto non_self_connections = filter([&](auto x)
                                                 { return x.lock() != hdl.lock(); },
                                                 m_connections);
        for_each([&](auto conn)
                 { m_server.send(conn, msg); },
                 non_self_connections);
    }

    void run(uint16_t port)
    {
        m_server.listen(port);
        m_server.start_accept();
        m_server.run();
    }

private:
    server m_server;
    Vector<connection_hdl> m_connections;
};

int main()
{
    info("Starting WebSocket server");

    broadcast_server server;
    server.run(9002);
}