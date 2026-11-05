// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.

// Tetromino 정의
enum class TetrominoType {
    I,
    O,
    T,
    S,
    Z,
    J,
    L
};

#include "framework.h"
#include "WindowsProject1.h"
#include <cmath>
#include <stdio.h>
#include <iostream>

#define MAX_LOADSTRING 100



// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND g_hWnd;
HDC g_hdc;
HDC memDC;
HBITMAP memBM;
HGDIOBJ oldBM;
TetrominoType currentType;

unsigned int prevTick;
float relapsed;
constexpr int boardWidth = 10;
constexpr int boardHeight = 20;
constexpr int cellSize = 25; // 25px

int board[boardHeight][boardWidth] = {};
int shape[4][4] = {};
int pieceX;
int pieceY;
int bottomY;
int mostLeftX;
int mostRightX;

bool isDownPressed = false;
bool isUpPressed = false;
bool isRightPressed = false;
bool isLeftPressed = false;
bool isTetrisReady;
bool isCollision;

// 전역 함수:
void Init();
void Update(float dt);
void Render();
void Release();
void SpawnTetromino();
bool CheckCollision();

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/// <summary>
/// 오른쪽 Boundary 를 next move 에 넘을 것인지 체크합니다.
/// 넘는다면 True 를 반환합니다.
/// </summary>
/// <returns></returns>
bool CheckRIghtBoundary() {
    if (mostRightX + pieceX + 1 >= boardWidth) {
        return true;
    }
    return false;
}

/// <summary>
/// 왼쪽 Boundary 를 next move 에 넘을 것인지 체크합니다.
/// 넘는다면 True 를 반환합니다.
/// </summary>
/// <returns></returns>
bool CheckLeftBoundary() {
    if (mostLeftX + pieceX - 1 < 0) {
        return true;
    }
    return false;
}


// TODO: Implement CheckCollision
bool CheckCollision() {
    
    // calculation:
    // bottomY + piece Y > boardHeight => 1.
    // [bottomY + piece Y][pieceX] 
    
    // 1. Next move Out of bound (piece Y + current type of tetromino bottomY exceeds the boundary)
    if (bottomY + pieceY + 1 >= boardHeight) {
        return true;
    }

    // 2. Next move Calculated coordinate is already occupied on board variable
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape[j][i] == 1 && board[pieceY + j + 1][pieceX + i] == 1) {
                return true;
            }
        }
    }

    return false;
}

void Init() {
    prevTick = GetTickCount();
    g_hdc = GetDC(g_hWnd);
    isTetrisReady = true;
    isCollision = false;
    relapsed = 0;
    // Double buffering
    memDC = CreateCompatibleDC(g_hdc);
    memBM = CreateCompatibleBitmap(g_hdc, 800, 600);
    oldBM = SelectObject(memDC, memBM);
}

void Update(float dt) {
    relapsed += dt;

    if (isTetrisReady) {
        SpawnTetromino();
        isTetrisReady = false;
    }

    // handle user input
    if (isRightPressed == true && !CheckRIghtBoundary()) {
        pieceX += 1;
        isRightPressed = false;
    }

    if (isLeftPressed == true && !CheckLeftBoundary()) {
        pieceX -= 1;
        isLeftPressed = false;
    }
    

    if (isDownPressed == true && !CheckCollision()) {
        pieceY += 1;
        isDownPressed = false;
    }

    if (relapsed >= 1) {
        if (CheckCollision()) {

            // When spawned tetromino is collide with Barrier OR locked tetromino
            // 1. Lock in tetromino into current position | currentPos = pieceX pieceY into board
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (shape[j][i] == 1) {
                        board[pieceY + j][pieceX + i] = 1;
                    }
                }
            }

            // 2. Remove tetromino from the shape
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    shape[j][i] = 0;
                }
            }

            // 3. Spawn Tetromino
            isTetrisReady = true;
        }
        else {
            pieceY += 1;
        }
        relapsed -= 1;

    }
}

void Render() {
    RECT rect = { 0, 0, 800, 600 };
    FillRect(memDC, &rect, (HBRUSH)(COLOR_WINDOW + 1));

    // Tetris grid 생성
    for (int i = 0; i < boardWidth; i++) {
        for (int j = 0; j < boardHeight; j++) {
            if (board[j][i] == 1) {
                // Fill the rect
                RECT r = { 0 + i * cellSize, 0 + j * cellSize, (i + 1) * cellSize, (j + 1) * cellSize };
                FillRect(memDC, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            else {
                Rectangle(memDC, 0 + i * cellSize, 0 + j * cellSize, (i + 1) * cellSize, (j + 1) * cellSize);
            }
        }
    }

    // Tetris shape render
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape[j][i] == 1) {
                // Fill the rect
                int left = pieceX + i;
                int right = pieceX + i + 1;
                int top = pieceY + j;
                int bottom = pieceY + j + 1;
                RECT r = { left * cellSize, top * cellSize, right * cellSize, bottom * cellSize };
                FillRect(memDC, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
        }
    }
    

    // WINGDIAPI BOOL  WINAPI BitBlt( _In_ HDC hdc, _In_ int x, _In_ int y, _In_ int cx, _In_ int cy, _In_opt_ HDC hdcSrc, _In_ int x1, _In_ int y1, _In_ DWORD rop);
    BitBlt(g_hdc, 0, 0, 800, 600, memDC, 0, 0, SRCCOPY);
}

void Release() {
    SelectObject(memDC, oldBM);
    DeleteObject(memBM);
    DeleteDC(memDC);
    ReleaseDC(g_hWnd, g_hdc);
}

void SpawnTetromino() {
    currentType = TetrominoType::S;
    pieceX = boardWidth / 2;
    pieceY = 0;
    // shape 값 0 으로 초기화
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            shape[j][i] = 0;
        }
    }

    // currentType 에 따라 shape 변형
    switch (currentType) {
    case TetrominoType::O:
        shape[1][1] = 1;
        shape[1][2] = 1;
        shape[2][1] = 1;
        shape[2][2] = 1;
        break;
    case TetrominoType::S:
        shape[1][1] = 1;
        shape[1][2] = 1;
        shape[2][2] = 1;
        shape[2][3] = 1;
        break;
    }

    // find bottomY
    bottomY = -1;
    for (int i = 0; i < 4; i++) { 
        for (int j = 0; j < 4; j++) { // j corresponds to height
            if (shape[j][i] == 1 && j > bottomY) {
                bottomY = j;
            }
        }
    }

    mostLeftX = boardWidth + 1;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) { // j corresponds to height
            if (shape[j][i] == 1 && i < mostLeftX) {
                mostLeftX = i;
            }
        }
    }

    mostRightX = -1;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) { // j corresponds to height
            if (shape[j][i] == 1 && i > mostRightX) {
                mostRightX = i;
            }
        }
    }
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{ 
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg = {};

    Init();

    // 기본 메시지 루프입니다:
    while (msg.message != WM_QUIT)
    {
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        if (msg.message == WM_QUIT) {
            break;
        }

        DWORD currentTick = GetTickCount();
        float dt = (currentTick - prevTick) / 1000.0f;
        prevTick = currentTick;
        Update(dt);
        printf("%f", dt);
        Render();
    }

    return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    g_hWnd = hWnd;
    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        // TODO: 여기에 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        Release();
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        if (wParam == VK_DOWN) {
            isDownPressed = true;
        }
        if (wParam == VK_RIGHT) {
            isRightPressed = true;
        }
        if (wParam == VK_LEFT) {
            isLeftPressed = true;
        }
        break;
    case WM_KEYUP:
        break;
    case WM_KILLFOCUS:
        isDownPressed = false;
        isUpPressed = false;
        isRightPressed = false;
        isLeftPressed = false;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}