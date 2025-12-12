#include "Quiz.h"
#include "Utils.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <cmath>
#include <algorithm>
#include <conio.h> 

// --- Helper Functions ---

// Ham tinh do dai hien thi thuc te cua chuoi UTF-8 (Tieng Viet)
// Giup can chinh thang hang ke ca khi co dau
int getVisibleLength(const string& s) {
    int len = 0;
    for (size_t i = 0; i < s.length(); ) {
        unsigned char c = s[i];
        int charLen = 1;
        if ((c & 0x80) == 0) charLen = 1;       // ASCII (1 byte)
        else if ((c & 0xE0) == 0xC0) charLen = 2; // 2 bytes
        else if ((c & 0xF0) == 0xE0) charLen = 3; // 3 bytes
        else if ((c & 0xF8) == 0xF0) charLen = 4; // 4 bytes

        i += charLen;
        len++; // Moi ky tu (du bao nhieu byte) chi tinh la 1 do rong hien thi
    }
    return len;
}

// Ham in can giua trong mot cot co do rong co dinh
void printCentered(string text, int width, bool isLast = false) {
    int visibleLen = getVisibleLength(text);
    if (visibleLen > width) {
        text = text.substr(0, width - 3) + "..."; // Cat bot neu dai qua
        visibleLen = width;
    }

    int padding = width - visibleLen;
    int leftPad = padding / 2;
    int rightPad = padding - leftPad;

    cout << string(leftPad, ' ') << text << string(rightPad, ' ');
    if (!isLast) cout << " | "; // Them duong ke doc phan chia
}

// Luu ket qua vao file results.txt
void saveResultToFile(const Candidate& cand, double score, double duration) {
    ofstream ghiFile("results.txt", ios::app);
    if (ghiFile) {
        time_t now = time(nullptr);
        tm local_tm;
        localtime_s(&local_tm, &now);
        ghiFile << cand.name << ";" << cand.className << ";" << cand.idNum << ";"
            << score << ";" << duration << ";"
            << put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << "\n";
    }
}

// Hien thi bang xep hang (DA FIX CAN GIUA & THANG HANG)
void showRanking() {
    clearScreen();
    cout << setfill(' ');

    ifstream fin("results.txt");
    if (!fin) {
        setColor(RED); cout << "Chua co du lieu xep hang.\n"; setColor(WHITE);
        return;
    }

    struct Entry { string name, cls, id; double score, duration; string time; };
    vector<Entry> list;
    string line;

    while (getline(fin, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        Entry e; string tmp;
        getline(ss, e.name, ';'); getline(ss, e.cls, ';'); getline(ss, e.id, ';');
        getline(ss, tmp, ';'); e.score = stod(tmp);
        getline(ss, tmp, ';'); e.duration = stod(tmp);
        getline(ss, e.time);
        list.push_back(e);
    }
    fin.close();

    // Sap xep
    sort(list.begin(), list.end(), [](const Entry& a, const Entry& b) {
        if (a.score != b.score) return a.score > b.score;
        return a.duration < b.duration;
        });

    setColor(YELLOW); cout << "\n                                           ===== BANG XEP HANG =====\n"; setColor(WHITE);
    cout << endl;

    // Dinh nghia do rong cac cot
    int wTop = 5;
    int wName = 30;
    int wClass = 15;
    int wScore = 8;
    int wTime = 10;
    int wDate = 20;

    // IN TIEU DE
    setColor(LIGHT_AQUA);
    printCentered("Top", wTop);
    printCentered("Ho ten", wName);
    printCentered("Lop", wClass);
    printCentered("Diem", wScore);
    printCentered("Giay", wTime); // Doi ten cho ngan gon
    printCentered("Ngay thi", wDate, true);
    cout << "\n";

    // Ke duong ngang
    cout << string(wTop + wName + wClass + wScore + wTime + wDate + 15, '-') << "\n";
    setColor(WHITE);

    // IN DU LIEU
    for (size_t i = 0; i < list.size(); ++i) {
        if (i == 0) setColor(LIGHT_RED); else if (i == 1) setColor(YELLOW); else if (i == 2) setColor(GREEN);
        else setColor(WHITE);

        printCentered(to_string(i + 1), wTop);
        printCentered(list[i].name, wName);
        printCentered(list[i].cls, wClass);

        // Xu ly diem so: Neu tron (10.0) thi in 10, le (9.5) thi in 9.5
        stringstream ssScore;
        if (list[i].score == (int)list[i].score) ssScore << (int)list[i].score;
        else ssScore << fixed << setprecision(1) << list[i].score;
        printCentered(ssScore.str(), wScore);

        stringstream ssTime; ssTime << fixed << setprecision(1) << list[i].duration;
        printCentered(ssTime.str(), wTime);

        printCentered(list[i].time, wDate, true);
        cout << "\n";
    }
    setColor(WHITE);
}

// --- Quiz Class Implementation ---

// Load cau hoi tu file questions.txt
bool Quiz::loadFromFile(const string& filename) {
    ifstream fin(filename);
    if (!fin) return false;
    questions.clear();
    string qtext, line;
    int id = 1;
    while (getline(fin, qtext)) {
        if (qtext.empty()) continue;
        array<string, 4> opts{};
        for (int i = 0; i < 4; i++) {
            if (!getline(fin, line)) break;
            if (line.size() >= 3 && line[1] == '.') opts[i] = line.substr(3);
            else opts[i] = line;
        }
        string ans; getline(fin, ans);
        char corr = ans.empty() ? 'A' : toupper(ans[0]);
        getline(fin, line); // Doc dong phan cach "---"
        questions.push_back(make_unique<MCQ>(id++, qtext, opts, corr, 1.0));
    }
    return !questions.empty();
}

void Quiz::randomizeAndSelect(int count) {
    if (questions.empty()) return;
    random_device rd; mt19937 g(rd());
    shuffle(questions.begin(), questions.end(), g);
    if (count > 0 && count < questions.size()) questions.resize(count);
}

void Quiz::enterCandidate() {
    clearScreen();
    setColor(AQUA); cout << "=== NHAP THONG TIN ===\n"; setColor(WHITE);
    cout << "Ho ten: "; cin.ignore(); getline(cin, cand.name);
    cout << "Lop: "; getline(cin, cand.className);
    cout << "MSSV: "; getline(cin, cand.idNum);
}

void Quiz::start(int totalMinutes) {
    totalDuration = chrono::minutes(totalMinutes);
    startTime = chrono::system_clock::now();
    answers.assign(questions.size(), 0);
    clearScreen();
}

void Quiz::takeTestLoop() {
    int currentQ = 0;
    bool finished = false;

    while (!finished && currentQ < questions.size()) {
        clearScreen();
        setColor(GRAY);
        cout << "Thi sinh: " << cand.name << " | MSSV: " << cand.idNum << endl;
        cout << string(60, '=') << endl;
        setColor(WHITE);

        setColor(LIGHT_AQUA);
        cout << "Cau " << (currentQ + 1) << "/" << questions.size() << ": " << questions[currentQ]->getText() << endl;
        setColor(WHITE);

        questions[currentQ]->print(5, 6);

        if (answers[currentQ] != 0) {
            gotoxy(0, 12);
            setColor(GREEN);
            cout << " -> Ban da chon: " << (answers[currentQ] == 'S' ? "BO QUA" : string(1, answers[currentQ])) << endl;
            setColor(WHITE);
        }

        gotoxy(0, 15);
        cout << string(60, '-') << endl;
        cout << "[A/B/C/D]: Chon | [S]: Skip | [F]: Nop bai\n";
        cout << "[<-]: Cau truoc  | [->]: Cau sau";

        while (true) {
            auto now = chrono::system_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(now - startTime);
            auto remaining = totalDuration - elapsed;

            if (remaining <= chrono::seconds(0)) {
                finished = true;
                gotoxy(0, 18); setColor(RED); cout << "HET GIO LAM BAI!"; Sleep(2000);
                break;
            }

            long long m = remaining.count() / 60;
            long long s = remaining.count() % 60;

            gotoxy(55, 1);
            setColor(YELLOW);
            cout << "TIME: " << setfill('0') << setw(2) << m << ":" << setw(2) << s;
            setColor(WHITE);

            if (_kbhit()) {
                char ch = _getch();
                if (ch == -32) {
                    ch = _getch();
                    if (ch == 75 && currentQ > 0) { currentQ--; break; }
                    if (ch == 77 && currentQ < questions.size() - 1) { currentQ++; break; }
                    continue;
                }

                ch = toupper(ch);
                if ((ch >= 'A' && ch <= 'D') || ch == 'S') {
                    answers[currentQ] = ch;
                    if (currentQ < questions.size() - 1) currentQ++;
                    break;
                }
                else if (ch == 'F') {
                    gotoxy(0, 18);
                    setColor(LIGHT_RED);
                    cout << "Ban chac chan nop bai? (Y/N): ";
                    char cf = toupper(_getch());
                    if (cf == 'Y') { finished = true; break; }
                    else {
                        gotoxy(0, 18); cout << "                                   ";
                    }
                    setColor(WHITE);
                }
                else {
                    gotoxy(0, 18);
                    setColor(RED);
                    cout << ">> PHIM KHONG HOP LE! (Chon A, B, C, D, S, F)";
                    setColor(WHITE);
                    Sleep(500);
                    gotoxy(0, 18); cout << "                                             ";
                }
            }
            Sleep(50);
        }
    }
    endTime = chrono::system_clock::now();
}

void Quiz::gradeAndShowResult() {
    clearScreen();
    int correctCount = 0;
    history.clear();
    for (size_t i = 0; i < questions.size(); i++) {
        char uAns = (i < answers.size()) ? answers[i] : 0;
        bool isCorrect = questions[i]->isCorrect(uAns);
        if (isCorrect) correctCount++;
        history.push_back({ questions[i]->getId(), questions[i]->getText(), uAns, questions[i]->getCorrect(), isCorrect });
    }

    double finalScore = 0.0;
    if (!questions.empty()) {
        finalScore = ((double)correctCount / questions.size()) * 10.0;
        finalScore = round(finalScore * 10.0) / 10.0;
    }

    setColor(AQUA);
    cout << "\n=== KET QUA ===\n"; setColor(WHITE);
    cout << "Thi sinh: " << cand.name << "\n";
    cout << "Cau dung: " << correctCount << "/" << questions.size() << "\n";
    setColor(YELLOW);
    cout << "DIEM SO: " << finalScore << " / 10.0\n"; setColor(WHITE);

    chrono::duration<double> diff = endTime - startTime;
    saveResultToFile(cand, finalScore, diff.count());

    cout << "\nXem lai bai lam? (Y/N): ";
    char c; cin >> c;
    if (toupper(c) == 'Y') reviewMistakes();
}

void Quiz::reviewMistakes() {
    clearScreen();
    setColor(LIGHT_RED); cout << "--- CAU SAI ---\n"; setColor(WHITE);
    bool hasWrong = false;
    for (auto& h : history) {
        if (!h.isCorrect) {
            hasWrong = true;
            cout << "Cau: " << h.qText << "\n";
            cout << "   Ban chon: " << (h.userAns == 0 ? '_' : h.userAns)
                << " -> Dap an dung: " << h.correctAns << "\n";
            cout << string(40, '-') << "\n";
        }
    }
    if (!hasWrong) cout << "Chuc mung! Ban dung tat ca cac cau.\n";
    pauseConsole();
}

// --- ADMIN FEATURES ---

bool Quiz::checkPassword() {
    clearScreen();
    setColor(PURPLE); cout << "=== ADMIN LOGIN ===\n"; setColor(WHITE);
    cout << "Password: ";
    string pass = ""; char ch;
    while (true) {
        ch = _getch();
        if (ch == 13) break;
        if (ch == 8) {
            if (!pass.empty()) { pass.pop_back(); cout << "\b \b"; }
        }
        else { pass += ch; cout << "*"; }
    }
    if (pass == ADMIN_PASS) return true;
    setColor(RED); cout << "\nSai mat khau!\n"; Sleep(1000); setColor(WHITE);
    return false;
}

void Quiz::viewQuestions() {
    clearScreen();
    if (!loadFromFile("questions.txt")) { cout << "Khong co du lieu!\n"; pauseConsole(); return; }
    for (auto& q : questions) {
        cout << "[" << q->getId() << "] " << q->getText() << "\n   -> Dap an: " << q->getCorrect() << "\n\n";
    }
    pauseConsole();
}

void Quiz::addQuestion() {
    clearScreen();
    setColor(GREEN); cout << "--- THEM CAU HOI MOI ---\n"; setColor(WHITE);

    string q, a, b, c, d;
    cout << "Noi dung cau hoi: "; cin.ignore(); getline(cin, q);
    cout << "Dap an A: "; getline(cin, a);
    cout << "Dap an B: "; getline(cin, b);
    cout << "Dap an C: "; getline(cin, c);
    cout << "Dap an D: "; getline(cin, d);

    char ansChar;
    string input;
    bool valid = false;
    do {
        cout << "Dap an dung (A/B/C/D): ";
        cin >> input;
        if (!input.empty()) {
            ansChar = toupper(input[0]);
            if (ansChar >= 'A' && ansChar <= 'D') {
                valid = true;
            }
        }
        if (!valid) {
            setColor(RED);
            cout << ">> Loi: Chi duoc nhap A, B, C hoac D. Vui long nhap lai!\n";
            setColor(WHITE);
        }
    } while (!valid);

    ofstream f("questions.txt", ios::app);
    if (f) {
        f << q << "\n";
        f << "A. " << a << "\nB. " << b << "\nC. " << c << "\nD. " << d << "\n";
        f << ansChar << "\n---\n";
        f.close();
        setColor(GREEN); cout << "Da them thanh cong!\n"; setColor(WHITE);
    }
    else {
        cout << "Loi mo file!\n";
    }
    pauseConsole();
}

void Quiz::deleteQuestion() {
    clearScreen();
    if (!loadFromFile("questions.txt")) return;
    viewQuestions();

    cout << "Nhap ID cau hoi muon xoa (0 de huy): ";
    int delId; cin >> delId;
    if (delId == 0) return;

    auto it = remove_if(questions.begin(), questions.end(), [delId](auto& q) { return q->getId() == delId; });
    if (it != questions.end()) {
        questions.erase(it, questions.end());
        ofstream f("questions.txt", ios::trunc);
        for (auto& q : questions) q->writeToFile(f);
        f.close();
        setColor(GREEN); cout << "Da xoa thanh cong!\n"; setColor(WHITE);
    }
    else {
        cout << "Khong tim thay ID.\n";
    }
    pauseConsole();
}

void Quiz::clearLeaderboard() {
    cout << "Xoa toan bo bang xep hang? (Y/N): ";
    char c; cin >> c;
    if (toupper(c) == 'Y') {
        ofstream f("results.txt", ios::trunc);
        f.close();
        cout << "Da xoa.\n";
    }
    pauseConsole();
}

void Quiz::adminPanel() {
    if (!checkPassword()) return;
    int c;
    do {
        clearScreen();
        setColor(PURPLE); cout << "=== ADMIN PANEL ===\n"; setColor(WHITE);
        cout << "1. Xem danh sach cau hoi\n";
        cout << "2. Them cau hoi moi\n";
        cout << "3. Xoa cau hoi\n";
        cout << "4. Xoa bang xep hang\n";
        cout << "5. Quay lai\n";
        cout << "Chon: ";
        if (!(cin >> c)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        switch (c) {
        case 1: viewQuestions(); break;
        case 2: addQuestion(); break;
        case 3: deleteQuestion(); break;
        case 4: clearLeaderboard(); break;
        }
    } while (c != 5);
}
