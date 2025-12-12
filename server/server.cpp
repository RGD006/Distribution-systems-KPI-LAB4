#include "server.h"
#include <array>
#include <iostream>
#include <string>
#include <strsafe.h>
#include <tchar.h>
#include <vector>
#include <windows.h>

constexpr size_t fileBufferSize = 512;
constexpr char *empty = "";

std::vector<HANDLE> tFile;
std::vector<HANDLE> hFileMaps;
std::array<HANDLE, 2> hFile;
std::array<LPCTSTR, 2> fileNames = {TEXT("sample_file1"), TEXT("sample_file2")};

static DWORD WINAPI ListenFileMap(LPVOID fileObject) {
  auto file = *reinterpret_cast<HANDLE *>(fileObject);

  if (!file) {
    std::cout << "Can't find file" << std::endl;
    return -1;
  }

  LPVOID buff;

  buff = MapViewOfFile(file, FILE_MAP_ALL_ACCESS, 0, 0, 0);

  while (1) {
    if (!buff) {
      continue;
    }

    if (strlen(LPCSTR(buff)) > 0) {
      std::cout << "Message in file: " << LPCSTR(buff) << " "
                << strlen(LPCSTR(buff)) << std::endl;

      CopyMemory(buff, empty, fileBufferSize);
    }
  }

  return 0;
}

Server::Server() {
  for (size_t iHandle = 0; iHandle < fileNames.size(); iHandle++) {

    HANDLE hMap =
        CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0,
                          fileBufferSize, fileNames[iHandle]);

    if (!hMap) {
      std::cout << "CreateFileMapping error: " << GetLastError() << std::endl;
      exit(1);
    }

    hFileMaps.push_back(hMap);

    tFile.push_back(CreateThread(nullptr, 4096, ListenFileMap,
                                 &hFileMaps[iHandle], 0, nullptr));

    std::cout << "Create fileMap: " << fileNames[iHandle] << std::endl;
  }
}

void Server::run() {
  std::cout << "Listen fileMaps for 10 seconds" << std::endl;
  Sleep(10 * 1000);

  for (auto &filemaps : tFile) {
    TerminateThread(filemaps, 0);
  }
}

void Server::stop() {}