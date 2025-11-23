#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <map>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

constexpr LPCSTR DEFAULT_PORT = "27015";

class Server {
private:
  std::map<SOCKET, size_t> connections;
  WSADATA socketServer;
  SOCKET listenSocket;
  HANDLE connectionHandle;
  HANDLE serverHandle;
  SOCKADDR_IN addr;

public:
  Server();
  void run();
  void stop();
  friend DWORD WINAPI ServerConnectionHandler(LPVOID serverClass);
  friend DWORD WINAPI ServerHandler(LPVOID serverClass);
};