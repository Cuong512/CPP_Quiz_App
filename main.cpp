#include <iostream>
#include "Quiz.h"
#include "Utils.h"

using namespace std;

void startNewQuiz() {
    Quiz quiz;
    if (!quiz.loadFromFile("questions.txt")) {
        setColor(RED);
        cout << "Loi: Khong tim thay file questions.txt!\n";
        setColor(WHITE);
        pauseConsole(); return;
    }
    quiz.enterCandidate();

    int totalQ = quiz.getTotalQuestions();
    int num;
    do {
        cout << "He thong co " << totalQ << " cau. Ban muon thi bao nhieu cau? (0=Tat ca): ";
        if (!(cin >> num)) { cin.clear(); cin.ignore(1000, '\n'); continue; }
    } while (num < 0 || num > totalQ);

    if (num > 0) quiz.randomizeAndSelect(num);
    else quiz.randomizeAndSelect(totalQ);

    int min = 2; // Thoi gian mac dinh
    quiz.start(min);
    quiz.takeTestLoop();
    quiz.gradeAndShowResult();
}

int main() {
    // Cai dat Tieng Viet
    setupConsole(); // Goi ham setup tu Utils.h

    Quiz adminQuiz; // Doi tuong dung rieng cho Admin
    int choice = 0;

    do {
        clearScreen();
        setColor(AQUA);
        cout << "\n===== TRAC NGHIEM PRO =====\n";
        setColor(WHITE);
        cout << "1. Bat dau thi\n";
        cout << "2. Bang xep hang\n";
        cout << "3. Admin Panel (Quan ly cau hoi)\n"; // Gom gon
        cout << "4. Thoat\n";
        cout << string(30, '-') << "\n";
        cout << "Chon: ";

        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        switch (choice) {
        case 1: startNewQuiz(); break;
        case 2: showRanking(); pauseConsole(); break;
        case 3: adminQuiz.adminPanel(); break; // Vao thang Admin Panel
        case 4:
            // Sua lai thong bao khong dau de tranh loi hien thi neu chua set UTF-8
            setColor(YELLOW); cout << "\nTam biet! Chuc ban hoc tot!\n"; setColor(WHITE);
            break;
        default: cout << "Chon sai!\n"; Sleep(500);
        }
    } while (choice != 4);

    return 0;
}