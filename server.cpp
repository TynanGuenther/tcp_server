#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int main() {
    const int PORT_NUM = 8080;
    bool running = true;

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

    while(running){
	sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int client_fd = accept(server_fd, (sockaddr*)&client_addr, &addr_len);
	std::cout << "checking client\n";
	if (client_fd < 0) {
	    perror("accept");
	    close(server_fd);
	    close(client_fd);
	    return 1;
	}

	char client_addr_str[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &client_addr.sin_addr, client_addr_str, sizeof(client_addr_str)) == NULL) {
	    perror("inet_ntop");
	    close(client_fd);
	    close(server_fd);
	    return 1;
	}
	int client_port_num = ntohs(client_addr.sin_port);

	std::cout << "Port " << client_port_num << " Address: " << client_addr_str << std::endl;

	close(client_fd);
    }
    close(server_fd);

    return 0;
}

