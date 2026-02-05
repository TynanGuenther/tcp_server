#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <csignal>

volatile sig_atomic_t keep_running = 1;
void handle_sigint(int) {
    keep_running = 0;
}

int main() {
    const int PORT_NUM = 8080;

    signal(SIGINT, handle_sigint);

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

    fd_set master_set;
    fd_set read_set;

    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    int max_fd = server_fd;

    while(keep_running){
	read_set = master_set;

	int ready = select(max_fd + 1, &read_set, nullptr, nullptr, nullptr);
	if (ready < 0) {
	    if (errno == EINTR)
		continue;
	    perror("select");
	    break;
	}

	for (int fd = 0; fd <= max_fd; fd++) {
	    if (!FD_ISSET(fd, &read_set)) {
		continue;
	    }
	    if (fd == server_fd) {
		sockaddr_in client_addr{};
		socklen_t addr_len = sizeof(client_addr);
		
		int client_fd = accept(server_fd, (sockaddr*)&client_addr, &addr_len);
		if (client_fd < 0) {
		    perror("accept");
		    continue;
		}
		FD_SET(client_fd, &master_set);
		if (client_fd > max_fd)
		    max_fd = client_fd;
		std::cout << "New Client Connected!\n";
	    } else {
		char recieve[1024];
		ssize_t bytes = recv(fd, recieve, sizeof(recieve) - 1, 0);

		if (bytes <= 0){
		    if (bytes <0)
			perror("recv");

		    close(fd);
		    FD_CLR(fd, &master_set);
		    std::cout << "Client disconnected\n";
		} else {
		    recieve[bytes] = '\0';
		    std::cout << "Recieved: " << recieve << std::endl;

		    const char reply[] = "Message recieved\n";
		    send(fd, reply, strlen(reply), 0);
		}
	    }
	}
    }
    std::cout << "\nShutting Down Server...\n";

    for (int fd = 0; fd <= max_fd; fd++) {
	if (FD_ISSET(fd, &master_set)){
	    close(fd);
	}
    }

    close(server_fd);

    return 0;
}

