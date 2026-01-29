#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int main() {
    const int PORT_NUM = 8080;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
	perror("socket");
	return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT_NUM);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
	perror("bind");
	close(server_fd);
	return 1;
    }

    if (listen(server_fd, 5) < 0) {
	perror("listen");
	close(server_fd);
	return 1;
    }

    std::cout << "Server listening on port " << PORT_NUM << "...\n";

    //always block for now
    while(true) {
	pause();
    }

    close(server_fd);
    return 0;

    return 0;
}

