#pragma once
#define NOMINMAX 
#include <iostream>
#include <windows.h> 
#include <limits>
#include <string>
#include <conio.h>
#include <vector>

using namespace std;

// Dinh nghia mau sac
enum Color {
    BLACK = 0, BLUE = 1, GREEN = 2, AQUA = 3, RED = 4,
    PURPLE = 5, YELLOW = 6, WHITE = 7, GRAY = 8,
    LIGHT_BLUE = 9, LIGHT_GREEN = 10, LIGHT_AQUA = 11, LIGHT_RED = 12,
    LIGHT_PURPLE = 13, LIGHT_YELLOW = 14, BRIGHT_WHITE = 15
};

// Thiet lap man hinh Console
inline void setupConsole() {
    // 1. Thiet lap bang ma UTF-8 cho Tieng Viet
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    // 2. Dat kich thuoc man hinh co dinh (120 cot, 30 dong)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT rect = { 0, 0, 119, 29 }; // 120x30
    SetConsoleWindowInfo(hConsole, TRUE, &rect);

    COORD bufferSize = { 120, 30 };
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    // 3. An con tro chuot
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    // 4. Dat tieu de
    SetConsoleTitle(L"HE THONG THI TRAC NGHIEM PRO MAX");
}

inline void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

inline void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

inline void clearScreen() {
    COORD topLeft = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;
    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
    FillConsoleOutputAttribute(console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE, screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
    SetConsoleCursorPosition(console, topLeft);
}

inline void pauseConsole() {
    gotoxy(2, 28);
    setColor(GRAY);
    cout << ">> Nhan Enter de tiep tuc...";
    setColor(WHITE);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}