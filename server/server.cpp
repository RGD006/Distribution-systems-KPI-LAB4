#include "server.h"
#include <string>

constexpr size_t numberOfConnections = 100;

DWORD WINAPI ServerConnectionHandler(LPVOID serverClass) {
  Server *server = reinterpret_cast<Server *>(serverClass);
  int clientSize = sizeof(server->addr);

  for (size_t i = 0; i < numberOfConnections; i++) {
    SOCKET clientSocket =
        accept(server->listenSocket, (SOCKADDR *)&server->addr, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
      continue;
    }

    int clientId = static_cast<int>(server->connections.size());
    server->connections[clientSocket] = i;
  }

  return 0;
}

DWORD WINAPI ServerHandler(LPVOID serverClass) {
  Server *server = reinterpret_cast<Server *>(serverClass);
  bool show1 = false, show2 = false;

  while (1) {
    for (auto &el : server->connections) {
      switch (el.second) {
      case 0:
        if (!show1) {
          MessageBox(NULL, "Connected first user", "Success", MB_OK);
          send(el.first, "COM1", 4, NULL);
          show1 = true;
        }

        break;

      case 1:
        if (!show2) {
          MessageBox(NULL, "Connected second user", "Success", MB_OK);
          send(el.first, "COM2", 4, NULL);
          show2 = true;
        }
        break;

      default:
        break;
      }
    }
  }

  return 0;
}

Server::Server() {
  if (WSAStartup(MAKEWORD(2, 2), &socketServer) != 0) {
    MessageBox(NULL, "Can't startup WSADATA", "ERROR", MB_ICONERROR);
    exit(1);
  }

  ZeroMemory(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1111);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listenSocket == INVALID_SOCKET) {
    MessageBox(NULL, "Error creating listen socket", "ERROR", MB_ICONERROR);
    WSACleanup();
    exit(1);
  }

  if (bind(listenSocket, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR) {
    MessageBox(NULL, "Bind failed", "ERROR", MB_ICONERROR);
    closesocket(listenSocket);
    WSACleanup();
    exit(1);
  }

  if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
    MessageBox(NULL, "Listen failed", "ERROR", MB_ICONERROR);
    closesocket(listenSocket);
    WSACleanup();
    exit(1);
  }
}

void Server::run() {
  connectionHandle =
      CreateThread(NULL, 0, ServerConnectionHandler, this, 0, NULL);
  serverHandle = CreateThread(NULL, 0, ServerHandler, this, 0, NULL);
}

void Server::stop() { CloseHandle(connectionHandle); }