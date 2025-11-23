#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <string>
#include <windows.h>

#define WM_UPDATE_RENDER (WM_USER + 1)

LPCSTR CLASS_NAME = "SampleWindowClass";
HANDLE cHandle;
HWND hwnd;
uint32_t renderStyle = 0;

static DWORD WINAPI clientHandle(LPVOID connection) {
  char commandBuffer[4];
  recv((SOCKET)connection, commandBuffer, sizeof(commandBuffer), NULL);

  if (memcmp(commandBuffer, "COM1", sizeof(commandBuffer)) == 0) {
    renderStyle = 1;
  } else if (memcmp(commandBuffer, "COM2", sizeof(commandBuffer)) == 0) {
    renderStyle = 2;
  }

  PostMessage(hwnd, WM_UPDATE_RENDER, 0, 0);
  return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  switch (uMsg) {
  case WM_UPDATE_RENDER:
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_PAINT: {
    if (renderStyle == 1) {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);

      MEMORYSTATUSEX memInfo;
      memInfo.dwLength = sizeof(MEMORYSTATUSEX);
      GlobalMemoryStatusEx(&memInfo);
      DWORDLONG ramMB = memInfo.ullTotalPhys / (1024 * 1024);

      int virtualWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);

      int remoteControl = GetSystemMetrics(SM_REMOTECONTROL);

      int horzRes = GetDeviceCaps(hdc, HORZRES);

      std::string text = "Client 1\n"
                         "RAM: " +
                         std::to_string(ramMB) + " MB\n" +
                         "SM_CXVIRTUALSCREEN: " + std::to_string(virtualWidth) +
                         "\n" +
                         "SM_REMOTECONTROL: " + std::to_string(remoteControl) +
                         "\n" + "HORZRES: " + std::to_string(horzRes) + "\n";

      TextOutA(hdc, 10, 10, text.c_str(), text.size());

      EndPaint(hwnd, &ps);

    } else if (renderStyle == 2) {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);

      int cyEdge = GetSystemMetrics(SM_CYEDGE);

      int cxEdge = GetSystemMetrics(SM_CXEDGE);

      int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);

      std::string text = "Client 2\n"
                         "SM_CYEDGE: " +
                         std::to_string(cyEdge) + "\n" +
                         "SM_CXEDGE: " + std::to_string(cxEdge) + "\n" +
                         "LOGPIXELSX (DPI): " + std::to_string(dpiX) + "\n";

      TextOutA(hdc, 10, 10, text.c_str(), text.size());

      EndPaint(hwnd, &ps);
    } else {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
      EndPaint(hwnd, &ps);
    }
  }
    return 0;
  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine,
                    int nCmdShow) {
  WNDCLASS wc = {};
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);

  hwnd = CreateWindowEx(0, CLASS_NAME, "Client", WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr,
                        nullptr, hInstance, nullptr);

  if (!hwnd)
    return 0;

  ShowWindow(hwnd, nCmdShow);

  WSAData wsaData;
  WORD DLLVersion = MAKEWORD(2, 2);

  if (WSAStartup(DLLVersion, &wsaData) != 0) {
    MessageBox(hwnd, "WSAStartup failed", "Error", MB_ICONERROR);
    return 1;
  }

  SOCKADDR_IN addr;
  int sizeofaddr = sizeof(addr);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  addr.sin_port = htons(1111);
  addr.sin_family = AF_INET;

  SOCKET connection = socket(AF_INET, SOCK_STREAM, NULL);
  if (connect(connection, (SOCKADDR *)&addr, sizeof(addr)) != 0) {
    MessageBox(hwnd, "Can't connect to client", "Error", MB_ICONERROR);
    return 1;
  }

  cHandle = CreateThread(NULL, 0, clientHandle, (LPVOID)connection, 0, NULL);

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}