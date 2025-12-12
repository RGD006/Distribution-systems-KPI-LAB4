#include <WinSock2.h>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  WSAData wsaData;
  WORD DLLVersion = MAKEWORD(2, 2);

  if (WSAStartup(DLLVersion, &wsaData) != 0) {
    std::cout << "WSA startup failed" << std::endl;
    return 1;
  }

  SOCKADDR_IN addr;
  int sizeofaddr = sizeof(addr);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  addr.sin_port = htons(1111);
  addr.sin_family = AF_INET;

  SOCKET connection = socket(AF_INET, SOCK_STREAM, NULL);

  if (connect(connection, (SOCKADDR *)&addr, sizeof(addr)) != 0) {
    std::cout << "Can't connect to client" << std::endl;
    return 1;
  }

  char commandBuffer[4];
  recv((SOCKET)connection, commandBuffer, sizeof(commandBuffer), NULL);

  if (memcmp(commandBuffer, "COM1", sizeof(commandBuffer)) == 0) {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG ramMB = memInfo.ullTotalPhys / (1024 * 1024);
    const int virtualWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int remoteControl = GetSystemMetrics(SM_REMOTECONTROL);

    std::cout << "Client 1\n"
              << "RAM: " << std::to_string(ramMB) << " MB\n"
              << "SM_CXVIRTUALSCREEN: " << std::to_string(virtualWidth) << "\n"
              << "SM_REMOTECONTROL: " << std::to_string(remoteControl) << "\n";
  } else if (memcmp(commandBuffer, "COM2", sizeof(commandBuffer)) == 0) {
    const int cyEdge = GetSystemMetrics(SM_CYEDGE);
    const int cxEdge = GetSystemMetrics(SM_CXEDGE);

    std::cout << "Client 2\n"
                 "SM_CYEDGE: "
              << std::to_string(cyEdge) << "\n"
              << "SM_CXEDGE: " << std::to_string(cxEdge) << "\n";
  }

  return 0;
}