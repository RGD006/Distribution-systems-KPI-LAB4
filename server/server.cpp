#include "server.h"

DWORD WINAPI ServerHandle(LPVOID serverClass) {
  Server *server = reinterpret_cast<Server *>(serverClass);

  return 0;
}

Server::Server() {
  struct addrinfo *result;
  int iResult = WSAStartup(MAKEWORD(2, 2), &socketServer);

  if (iResult != 0) {
    MessageBox(NULL, "Can't startup WSADATA", "ERROR", MB_ICONERROR);
    exit(1);
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
  iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

  if (iResult != 0) {
    MessageBox(NULL, "getaddrinfo failed", "ERROR", MB_ICONERROR);
    WSACleanup();
    exit(1);
  }

  listenSocket =
      socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  if (listenSocket == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    MessageBox(NULL, "Error create listen socket", "ERROR", MB_ICONERROR);
    freeaddrinfo(result);
    WSACleanup();
    exit(1);
  }
}

void Server::run() {
  mainHandle = CreateThread(NULL, 0, ServerHandle, this, 0, NULL);
}

void Server::stop() { CloseHandle(mainHandle); }