#include <WinSock2.h>
#include <array>
#include <iostream>
#include <string>

std::array<LPCTSTR, 2> fileNames = {TEXT("sample_file1"), TEXT("sample_file2")};

int main(int argc, char **argv) {
  unsigned int client = 0;

  while (1) {
    std::cout << "Enter the number of client:" << std::endl
              << "Client 1 (sample_file1): -> 0" << std::endl
              << "Client 2 (sample_file2): -> 1" << std::endl;

    std::cin >> client;

    if (client < 2)
      break;
    else
      std::cout << "Wrong client number" << std::endl;
  }

  HANDLE hFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, fileNames[client]);

  if (!hFile) {
    std::cout << "Can't create file handler: " << GetLastError() << std::endl;
    return -1;
  }

  PVOID pBuf = MapViewOfFile(hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

  std::string message("message from: " + std::to_string(client) + " mailbox");

  CopyMemory(pBuf, message.c_str(), message.size());

  return 0;
}