#include "server.h"
#include <string>
#include <iostream>
#include <vector>

constexpr size_t numberOfConnections = 2;

Server::Server() {
  if (WSAStartup(MAKEWORD(2, 2), &socketServer) != 0) {
    std::cout << "Can't startup WSADATA" << std::endl;
    exit(1);
  }

  ZeroMemory(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1111);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (listenSocket == INVALID_SOCKET) {
    std::cout << "Error create listen socket" << std::endl;
    WSACleanup();
    exit(1);
  }

  if (bind(listenSocket, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR) {
    std::cout << "Bind failed" << std::endl;
    closesocket(listenSocket);
    WSACleanup();
    exit(1);
  }

  if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
    std::cout << "Listen failed" << std::endl;
    closesocket(listenSocket);
    WSACleanup();
    exit(1);
  }
}

void Server::run() {
  int clientSize = sizeof(addr);
  std::vector<SOCKET> clients(numberOfConnections);

  for (size_t i = 0; i < numberOfConnections; i++) {
    clients[i] = accept(listenSocket, (SOCKADDR *)&addr, &clientSize);

    switch (i) {
    case 0:
      send(clients[i], "COM1", 4, 0);
      std::cout << "send data to first client" << std::endl;
      break;
    case 1:
      send(clients[i], "COM2", 4, 0);
      std::cout << "send data to second client" << std::endl;
      break;
    default:
      break;
    }
  }

  std::cout << "close server" << std::endl;
}

void Server::stop() {}