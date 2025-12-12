#pragma once

#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>

constexpr LPCSTR DEFAULT_PORT = "27015";

class Server {
private:
  WSADATA socketServer;
  SOCKET listenSocket;
  HANDLE connectionHandle;
  HANDLE serverHandle;
  SOCKADDR_IN addr;

public:
  Server();
  void run();
  void stop();
};