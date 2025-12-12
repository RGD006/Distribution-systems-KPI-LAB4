#include "server.h"
#include <array>
#include <iostream>
#include <string>
#include <strsafe.h>
#include <tchar.h>
#include <vector>
#include <windows.h>

constexpr size_t mailBoxListenTimeout = 500;

std::vector<HANDLE> tSlots;
std::vector<HANDLE> hSlots;
std::array<LPCTSTR, 2> slotNames = {TEXT("\\\\.\\mailslot\\sample_mailslot1"),
                                    TEXT("\\\\.\\mailslot\\sample_mailslot2")};

static DWORD WINAPI ReadSlot(PVOID slot) {
  auto mailSlot = *reinterpret_cast<std::pair<LPCSTR, size_t> *>(slot);
  delete slot;

  if (!mailSlot.first) {
    std::cout << "Error: no MailSlot name" << std::endl;
    exit(1);
  }

  while (1) {
    DWORD cbMessage, cMessage, cbRead;
    BOOL fResult;
    LPTSTR lpszBuffer;
    TCHAR achID[80];
    DWORD cAllMessages;
    HANDLE hEvent;
    OVERLAPPED ov;

    cbMessage = cMessage = cbRead = 0;

    hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ExampleSlot"));
    if (NULL == hEvent)
      exit(1);
    ov.Offset = 0;
    ov.OffsetHigh = 0;
    ov.hEvent = hEvent;

    fResult = GetMailslotInfo(hSlots[mailSlot.second], // mailslot handle
                              (LPDWORD)NULL,  // no maximum message size
                              &cbMessage,     // size of next message
                              &cMessage,      // number of messages
                              (LPDWORD)NULL); // no read time-out

    if (!fResult) {
      printf("GetMailslotInfo failed with %d.\n", GetLastError());
      exit(1);
    }

    if (cbMessage == MAILSLOT_NO_MESSAGE) {
      continue;
    }

    cAllMessages = cMessage;

    while (cMessage != 0) // retrieve all messages
    {
      // Create a message-number string.

      StringCchPrintf((LPTSTR)achID, 80, TEXT("<Message #%d of %d>\n"),
                      cAllMessages - cMessage + 1, cAllMessages);

      // Allocate memory for the message.

      lpszBuffer = (LPTSTR)GlobalAlloc(
          GPTR, lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage);
      if (NULL == lpszBuffer) {
        std::cout << "lpszBuffer is NULL" << std::endl;
        exit(1);
      }
      lpszBuffer[0] = '\0';

      fResult = ReadFile(hSlots[mailSlot.second], lpszBuffer, cbMessage,
                         &cbRead, &ov);

      if (!fResult) {
        printf("ReadFile failed with %d.\n", GetLastError());
        GlobalFree((HGLOBAL)lpszBuffer);
        exit(1);
      }

      // Concatenate the message and the message-number string.

      StringCbCat(lpszBuffer,
                  lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage,
                  (LPTSTR)achID);

      // Display the message.

      _tprintf(TEXT("Contents of the mailslot: `%s`\n"), lpszBuffer);

      GlobalFree((HGLOBAL)lpszBuffer);

      fResult = GetMailslotInfo(hSlots[mailSlot.second], // mailslot handle
                                (LPDWORD)NULL,  // no maximum message size
                                &cbMessage,     // size of next message
                                &cMessage,      // number of messages
                                (LPDWORD)NULL); // no read time-out

      if (!fResult) {
        printf("GetMailslotInfo failed (%d)\n", GetLastError());
        exit(1);
      }
    }

    Sleep(500);
    CloseHandle(hEvent);
  }
}

static void MakeSlot(LPCSTR slotName) {
  hSlots.push_back(CreateMailslot(slotName, 0, MAILSLOT_WAIT_FOREVER, nullptr));

  if (hSlots.back() == INVALID_HANDLE_VALUE) {
    std::cout << "Can't craete MailSlot: " << GetLastError() << std::endl;
    exit(1);
  }

  auto *mailSlot = new std::pair<LPCSTR, size_t>(slotName, hSlots.size() - 1);

  tSlots.push_back(CreateThread(nullptr, 4096, ReadSlot, mailSlot, 0, nullptr));
  std::cout << "Create MailSlot: " << slotName << std::endl;
}

Server::Server() {
  for (const auto &slot : slotNames) {
    MakeSlot(slot);
  }
}

void Server::run() {
  std::cout << "Listen mailbox for 10 seconds" << std::endl;
  Sleep(10 * 1000);

  for (auto &mailbox : tSlots) {
    TerminateThread(mailbox, 0);
  }
}

void Server::stop() {}