# tcp_server
Simple TCP chat server written in C++ using select() to handle multipple clients concurrlently. The server uses and event driven loop, registers new clients as the connect, processes input only when sockets are ready, and supports a clean shutdown by handling SIGINT and closing all active sockets gracefully

## Build
```bash
make
