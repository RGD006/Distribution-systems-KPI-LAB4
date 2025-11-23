#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX     

#include <winsock2.h> 
#include <ws2tcpip.h>
#include <windows.h> 
#include <map>

constexpr LPCSTR DEFAULT_PORT = "27015";

class Server {
private:
  std::map<int, SOCKET> connections;
  WSADATA socketServer;
  SOCKET listenSocket;
  HANDLE mainHandle;
  struct addrinfo hints;

public:
  Server();
  void run();
  void stop();
  friend DWORD WINAPI ServerHandle(LPVOID serverClass);
};