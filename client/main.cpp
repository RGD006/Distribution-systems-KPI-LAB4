#include <WinSock2.h>
#include <array>
#include <iostream>
#include <string>

using PIPE = std::pair<LPCSTR, HANDLE>;

constexpr size_t pipeNumber = 2;
std::array<PIPE, pipeNumber> pipes = {
  std::make_pair(TEXT("\\\\.\\pipe\\pipe1"), nullptr),
  std::make_pair(TEXT("\\\\.\\pipe\\pipe2"), nullptr),
};

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

  pipes[client].second = CreateFile(pipes[client].first, GENERIC_READ | GENERIC_WRITE,
                                    0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (pipes[client].second == INVALID_HANDLE_VALUE) {
    std::printf("Can't open pipe %s (%lu)\n", pipes[client].first, GetLastError());
    return -1;
  }

  std::string message("Hello from pipe " + std::to_string(client + 1));

  if (!WriteFile(pipes[client].second, message.c_str(), message.size(), nullptr, nullptr)) {
    std::printf("Can't write to pipe: %lu\n", GetLastError());
  }

  return 0;
}