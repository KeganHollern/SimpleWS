#include "simplews.h"


#define ASIO_STANDALONE
#define WIN32_LEAN_AND_MEAN
#define _WEBSOCKETPP_CPP11_STL_

#include <Windows.h>
#include <stdint.h>
#include <functional>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <map>


#include <asio.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::connection_hdl Connection;
typedef websocketpp::server<websocketpp::config::asio> Endpoint;

class SimpleWS {
public:
    SimpleWS(std::string host, uint32_t port) {
        this->port = port;
        this->hostname = host;

        this->endpoint.set_open_handler(std::bind(&SimpleWS::onOpen, this, std::placeholders::_1));
        this->endpoint.set_close_handler(std::bind(&SimpleWS::onClose, this, std::placeholders::_1));
        this->endpoint.set_message_handler(std::bind(&SimpleWS::onMessage, this, std::placeholders::_1, std::placeholders::_2));

        this->endpoint.clear_access_channels(websocketpp::log::alevel::frame_header | websocketpp::log::alevel::frame_payload);
        this->endpoint.init_asio(&(this->eventLoop));
    }
    void Start() {
        this->endpoint.listen(this->hostname, std::to_string(this->port));
        this->endpoint.start_accept();

        this->pThread = new std::thread([this]() {
            this->endpoint.run();
        });
    }
    void Stop() {
        this->endpoint.stop_listening();
        this->endpoint.stop();
        this->pThread->join();
    }
    void Send(Connection conn, std::string messageData) {
        this->endpoint.send(conn, messageData, websocketpp::frame::opcode::text);
    }
    void Send(std::string messageData) {
        std::lock_guard<std::mutex> lock(this->connectionListMutex);

        for (auto conn : this->connections)
            this->Send(conn, messageData);
    }


    OnConnectEvent onConnectCallback;
    OnDisconnectEvent onDisconnectCallback;
    OnMessageEvent onMessageCallback;
private:

    void onOpen(Connection conn) {
        if(this->onConnectCallback) {
            this->onConnectCallback(&conn);
        }
    }
    void onClose(Connection conn) {
        if(this->onDisconnectCallback) {
            this->onDisconnectCallback(&conn);
        }
    }
    void onMessage(Connection conn, Endpoint::message_ptr msg) {
        if(this->onMessageCallback) {
            this->onMessageCallback(&conn, msg->get_payload().c_str());
        }
    }


    std::vector<Connection> connections;
    std::mutex connectionListMutex;

    Endpoint endpoint;

    std::string hostname;
    uint32_t port;
    asio::io_service eventLoop;

    std::thread* pThread;

};



// --- exported routines for library interaction

void sws_push(
        sws_server server,
        sws_client client,
        const char* message) {

    if(!server) return; // can't push message without referencing a server object

    SimpleWS* data = (SimpleWS*)server;
    if(client) {
        data->Send(*(std::weak_ptr<void>*)client, message);
    } else {
        data->Send(message);
    }
}
sws_server sws_start(
        const char* hostname,
        unsigned int port,
        OnConnectEvent on_connect,
        OnDisconnectEvent on_disconnect,
        OnMessageEvent on_message) {

    SimpleWS* data = new SimpleWS(hostname, port);
    data->onMessageCallback = on_message;
    data->onDisconnectCallback = on_disconnect;
    data->onConnectCallback = on_connect;
    data->Start();
    return data;
}