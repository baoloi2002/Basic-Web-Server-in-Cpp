#include <iostream>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

int main() {
    std::cout << "Attempting to create a server socket..." << std::endl;

    SOCKET wsocket;
    SOCKET new_wsocket;
#ifdef _WIN32
    WSADATA wsaData;
#endif
    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);
    int BUFFER_SIZE = 30720;

#ifdef _WIN32
    // Initialize wsadata
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Failed to initialize wsadata" << std::endl;
        return 0;
    }
#endif

    // Create a socket
    wsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (wsocket == INVALID_SOCKET) {
        std::cout << "Failed to create a socket" << std::endl;
        return 0;
    }

    // Bind the socket to an IP address and port
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(8080);

    if (bind(wsocket, (struct sockaddr*)&server, sizeof(server)) ==
        SOCKET_ERROR) {
        std::cout << "Failed to bind the socket" << std::endl;
        return 0;
    }

    // Listen for incoming connections
    if (listen(wsocket, 20) == SOCKET_ERROR) {
        std::cout << "Failed to listen for incoming connections" << std::endl;
        return 0;
    }

    std::cout << "Listening on 127.0.0.1:8080" << std::endl;

    int bytes_received = 0;
    while (true) {
        new_wsocket = accept(wsocket, (struct sockaddr*)&server, &server_len);
        if (new_wsocket == INVALID_SOCKET) {
            std::cout << "Failed to accept incoming connection" << std::endl;
            continue;
        }

        // Receive data from the client
        char buffer[BUFFER_SIZE] = {0};
        bytes_received = recv(new_wsocket, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            std::cout << "Failed to receive data from the client" << std::endl;
        }

        std::string serverMessage =
            "HTTP/1.1 200 OK\r\nContent-Type: "
            "text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";

        int bytesSent = 0;
        int totalBytesSent = 0;
        while (totalBytesSent < serverMessage.size()) {
            bytesSent = send(new_wsocket, serverMessage.c_str(),
                             serverMessage.size(), 0);
            if (bytesSent == SOCKET_ERROR) {
                std::cout << "Failed to send data to the client" << std::endl;
                break;
            }
            totalBytesSent += bytesSent;
        }
        std::cout << "Data sent to the client" << std::endl;

#ifdef _WIN32
        closesocket(new_wsocket);
#else
        close(new_wsocket);
#endif
    }

#ifdef _WIN32
    closesocket(wsocket);
    WSACleanup();
#else
    close(wsocket);
#endif

    return 0;
}
