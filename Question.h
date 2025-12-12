#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include "Utils.h"

using namespace std;

class Question {
protected:
    int id;
    string text;
    double points;
    char correct;

public:
    Question(int id_ = 0, string t_ = "", char corr = 'A', double p_ = 1.0)
        : id(id_), text(move(t_)), points(p_) {
        correct = toupper(corr);
    }
    virtual ~Question() = default;

    int getId() const { return id; }
    string getText() const { return text; }

    char getCorrect() const { return correct; }
    double getPoints() const { return points; }

    virtual bool isCorrect(char ans) const {
        return toupper(ans) == correct;
    }

    // Ham thuan ao (pure virtual)
    virtual void print(int x, int y) const = 0;
    virtual void writeToFile(ofstream& file) const = 0;
};

class MCQ : public Question {
    array<string, 4> options;
public:
    MCQ(int id_, string t_, const array<string, 4>& o, char corr, double p_)
        : Question(id_, move(t_), corr, p_), options(o) {
    }

    // --- HAM IN CAU HOI (DA FIX LOI HIEN THI DONG) ---
    void print(int x, int y) const override {
        setColor(BRIGHT_WHITE);

        // Bien theo doi vi tri dong hien tai (Dynamic Y)
        int currentY = y;

        for (size_t i = 0; i < options.size(); ++i) {
            char label = 'A' + i;

            // Dat con tro tai vi tri dong (duoc tinh toan lai sau moi lan in)
            gotoxy(x, currentY);

            // In nhan A, B, C, D mau vang
            setColor(YELLOW); cout << label << ". ";

            // In noi dung dap an mau trang
            setColor(WHITE);  cout << options[i];

            // --- FIX LOI DE DONG ---
            // Lay vi tri thuc te cua con tro sau khi in xong dap an nay
            // (Vi dap an dai co the da tu xuong dong)
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
                // Dap an tiep theo se bat dau o dong ke tiep cua vi tri hien tai
                currentY = csbi.dwCursorPosition.Y + 1;
            }
            else {
                // Truong hop du phong neu khong lay duoc API (rat hiem)
                currentY++;
            }
        }
    }

    void writeToFile(ofstream& file) const override {
        file << text << "\n";
        for (const auto& opt : options) file << opt << "\n";
        file << correct << "\n---\n";
    }
};