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

    // --- MOI THEM: Ham lay noi dung text cua dap an ---
    virtual string getAnswerText(char label) const { return ""; }

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

    // --- HIEN THUC HAM LAY TEXT ---
    string getAnswerText(char label) const override {
        label = toupper(label);
        if (label == 'S') return "BO QUA"; 
        if (label == 0) return "CHUA CHON";
        
        int index = label - 'A';
        if (index >= 0 && index < 4) {
            return options[index];
        }
        return "";
    }

    // --- HAM IN CAU HOI (GIU NGUYEN FIX HIEN THI) ---
    void print(int x, int y) const override {
        setColor(BRIGHT_WHITE);
        int currentY = y;

        for (size_t i = 0; i < options.size(); ++i) {
            char label = 'A' + i;
            gotoxy(x, currentY);
            setColor(YELLOW); cout << label << ". ";
            setColor(WHITE);  cout << options[i];

            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
                currentY = csbi.dwCursorPosition.Y + 1;
            }
            else {
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
