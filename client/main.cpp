#include <WinSock2.h>
#include <array>
#include <iostream>
#include <string>

std::array<LPCTSTR, 2> slotNames = {TEXT("\\\\.\\mailslot\\sample_mailslot1"),
                                    TEXT("\\\\.\\mailslot\\sample_mailslot2")};

BOOL WriteSlot(HANDLE hSlot, LPCTSTR lpszMessage) {
  BOOL fResult;
  DWORD cbWritten;

  fResult = WriteFile(hSlot, lpszMessage,
                      (DWORD)(lstrlen(lpszMessage) + 1) * sizeof(TCHAR),
                      &cbWritten, (LPOVERLAPPED)NULL);

  if (!fResult) {
    printf("WriteFile failed with %d.\n", GetLastError());

    if (GetLastError() == 6) {
      printf("Mailbox is not opened\n");
    }

    return FALSE;
  }

  printf("Slot written to successfully.\n");

  return TRUE;
}

int main(int argc, char **argv) {
  unsigned int client = 0;

  while (1) {
    std::cout << "Enter the number of client:" << std::endl
              << "Client 1: -> 0" << std::endl
              << "Client 2: -> 1" << std::endl;

    std::cin >> client;

    if (client < 2)
      break;
    else
      std::cout << "Wrong client number" << std::endl;
  }

  HANDLE hFile = CreateFile(slotNames[client], GENERIC_WRITE, FILE_SHARE_READ,
                            (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

  if (!hFile) {
    std::cout << "Can't create file handler" << std::endl;
    return -1;
  }

  std::string message("message from: " + std::to_string(client) + " mailbox");

  WriteSlot(hFile, message.c_str());

  return 0;
}