#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <windows.h>

// Имя класса окна
LPCSTR CLASS_NAME = "SampleWindowClass";

// Процедура обработки сообщений окна
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  switch (uMsg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
    EndPaint(hwnd, &ps);
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

  HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Client", WS_OVERLAPPEDWINDOW,
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

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}