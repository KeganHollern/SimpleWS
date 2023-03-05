# SimpleWS

A very basic websocket library for windows using WebsocketPP and ASIO. SSL not supported.

## Example usage

```c
sws_server svr;

void onConnect(sws_client client) {
    printf("client connected!\n");

    sws_push(svr, NULL, "a friend has joined!");
    sws_push(svr, client, "hi");
}

void main() {
    
    svr = sws_start("localhost", 8080, onConnect, NULL, NULL); // non-blocking
    
    while(1) {Sleep(100)};
}
```