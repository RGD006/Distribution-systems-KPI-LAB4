#include "server.h"
#include <array>
#include <iostream>
#include <string>
#include <strsafe.h>
#include <tchar.h>
#include <vector>
#include <windows.h>

static constexpr size_t pipeBufferSize = 512;
static constexpr size_t pipeNumber = 2;
static constexpr size_t pipeTimeoutMs = 5000;
// static constexpr char *empty = "";

using PIPE = std::pair<LPCSTR, HANDLE>;

std::array<HANDLE, pipeNumber> tPipes;
std::array<PIPE, pipeNumber> pipes = {
  std::make_pair(TEXT("\\\\.\\pipe\\pipe1"), nullptr),
  std::make_pair(TEXT("\\\\.\\pipe\\pipe2"), nullptr),
};

static DWORD WINAPI ListenPipe(PVOID pPipe) {
  const auto pipe = *reinterpret_cast<PIPE*>(pPipe);
  DWORD cbRead;
  char buff[pipeBufferSize];

  std::printf("Wait for connect to %s pipe\n", pipe.first);

  ConnectNamedPipe(pipe.second, nullptr);
  
  while (1) {
    BOOL success = ReadFile(pipe.second, buff, sizeof(buff), &cbRead, nullptr);
    
    if (success) {
      std::printf("Message from (%s): <%s>\n", pipe.first, buff);
    } else {
      switch (GetLastError()) {
        case 109:
          std::printf("Connection is closed\n");
          return 0;
        default:
          std::printf("ReadFile Erorr: %lu\n", GetLastError());
          return 1;
      }
    }
  }

  return 0;
}

Server::Server() {
  for (size_t i = 0; i < pipeNumber; i++) {
    pipes[i].second = CreateNamedPipe(pipes[i].first, 
                                PIPE_ACCESS_DUPLEX,
                                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                PIPE_UNLIMITED_INSTANCES,
                                pipeBufferSize, pipeBufferSize, pipeTimeoutMs, nullptr);

    if (pipes[i].second == INVALID_HANDLE_VALUE) {
      std::printf("Can't create pipe %s (%lu) \n", pipes[i].first, GetLastError());
    }
  }
}

void Server::run() {
  std::printf("Listen Pipes for 10 seconds\n");

  for (size_t i = 0; i < pipeNumber; i++) {
    tPipes[i] = CreateThread(nullptr, 4096, ListenPipe, &pipes[i], 0, nullptr);
  }

  Sleep(10 * 1000);


  for (auto &task : tPipes) {
    TerminateThread(task, 0);
  }
}

void Server::stop() {}