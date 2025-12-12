#pragma once
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include "Question.h"

using namespace std;

struct Candidate { string name; string className; string idNum; };
struct ReviewData { int qId; string qText; char userAns; char correctAns; bool isCorrect; };

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
    void takeTestLoop(); // Da sua lai hien thi
    void gradeAndShowResult();
    void reviewMistakes();

    // --- Admin Features ---
    void adminPanel();
    bool checkPassword();
    void viewQuestions();
    void deleteQuestion();
    void addQuestion(); // Chuyen vao day
    void clearLeaderboard();
};

void saveResultToFile(const Candidate& cand, double score, double duration);
void showRanking();