#pragma once
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include "Question.h"

using namespace std;

struct Candidate { string name; string className; string idNum; };

// --- CAP NHAT STRUCT NAY ---
struct ReviewData { 
    int qId; 
    string qText; 
    char userAns; 
    char correctAns; 
    string userAnsText;     // Noi dung dap an nguoi dung chon
    string correctAnsText;  // Noi dung dap an dung
    bool isCorrect; 
};

class Quiz {
private:
    vector<unique_ptr<Question>> questions;
    vector<char> answers;
    vector<ReviewData> history;
    Candidate cand;

    chrono::system_clock::time_point startTime, endTime;
    chrono::seconds totalDuration{ 0 };
    const string ADMIN_PASS = "admin";

public:
    bool loadFromFile(const string& filename);
    void enterCandidate();

    int getTotalQuestions() const { return questions.size(); }
    void randomizeAndSelect(int count);

    void start(int totalMinutes);
    void takeTestLoop(); 
    void gradeAndShowResult();
    void reviewMistakes(); // Ham nay se in ra text chi tiet

    // --- Admin Features ---
    void adminPanel();
    bool checkPassword();
    void viewQuestions();
    void deleteQuestion();
    void addQuestion(); 
    void clearLeaderboard();
};

void saveResultToFile(const Candidate& cand, double score, double duration);
void showRanking();
