#ifndef SIMPLEWS_LIBRARY_H
#define SIMPLEWS_LIBRARY_H

typedef void* sws_client;
typedef void* sws_server;
typedef void(__fastcall* OnConnectEvent)(sws_client client);
typedef void(__fastcall* OnDisconnectEvent)(sws_client client);
typedef void(__fastcall* OnMessageEvent)(sws_client client, const char* messageData);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SimpleWS_EXPORTS
#define LINK __declspec ( dllexport )
#else
#define LINK __declspec ( dllimport )
#endif

    // sws_push sends a message to the provided client.
    // send NULL as client for global broadcast.
    LINK void sws_push(
            sws_server server,
            sws_client client,
            const char* message);
    // sws_start starts the websocket server.
    // this is non-blocking.
    // callbacks for events can be registered.
    LINK sws_server sws_start(
            const char* hostname,
            unsigned int port,
            OnConnectEvent on_connect,
            OnDisconnectEvent on_disconnect,
            OnMessageEvent on_message);


#ifdef __cplusplus
};
#endif
#endif //SIMPLEWS_LIBRARY_H
