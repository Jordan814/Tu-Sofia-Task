#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <cctype>

using namespace std;

vector<string> undoStack;
const int UNDO_LIMIT = 10;
mutex mtx;
string sharedText;

// Създаване на речник за латинско-кирилски букви
map<char, char> latinToCyrillic = {
    {'A', 'А'}, {'B', 'Б'}, {'C', 'Ц'}, {'D', 'Д'}, {'E', 'Е'}, {'F', 'Ф'},
    {'G', 'Г'}, {'H', 'Х'}, {'I', 'И'}, {'J', 'Ј'}, {'K', 'К'}, {'L', 'Л'},
    {'M', 'М'}, {'N', 'Н'}, {'O', 'О'}, {'P', 'П'}, {'Q', 'Љ'}, {'R', 'Р'},
    {'S', 'С'}, {'T', 'Т'}, {'U', 'У'}, {'V', 'В'}, {'W', 'Ш'}, {'X', 'Ж'},
    {'Y', 'И'}, {'Z', 'З'}, {'a', 'а'}, {'b', 'б'}, {'c', 'ц'}, {'d', 'д'},
    {'e', 'е'}, {'f', 'ф'}, {'g', 'г'}, {'h', 'х'}, {'i', 'и'}, {'j', 'ј'},
    {'k', 'к'}, {'l', 'л'}, {'m', 'м'}, {'n', 'н'}, {'o', 'о'}, {'p', 'п'},
    {'q', 'љ'}, {'r', 'р'}, {'s', 'с'}, {'t', 'т'}, {'u', 'у'}, {'v', 'в'},
    {'w', 'ш'}, {'x', 'ж'}, {'y', 'и'}, {'z', 'з'}
};


string convertToCyrillic(const string& text) {
    string result;
    for (char ch : text) {

        if (latinToCyrillic.find(ch) != latinToCyrillic.end()) {
            result += latinToCyrillic[ch];
        } else {
            result += ch;
        }
    }
    return result;
}


string fixCapitalization(string text) {
    bool capitalizeNext = true;
    for (size_t i = 0; i < text.length(); ++i) {
        if (capitalizeNext && isalpha(text[i])) {
            text[i] = toupper(text[i]);
            capitalizeNext = false;
        }
        if (text[i] == '.') {
            capitalizeNext = true;
        }
    }
    return text;
}


void addToUndo(const string& text) {
    lock_guard<mutex> lock(mtx);
    if (undoStack.size() == UNDO_LIMIT) {
        undoStack.erase(undoStack.begin());
    }
    undoStack.push_back(text);
}


void processText() {
    lock_guard<mutex> lock(mtx);
    sharedText = convertToCyrillic(sharedText);
    sharedText = fixCapitalization(sharedText);
    addToUndo(sharedText);
}


void writeTextToFile() {
    lock_guard<mutex> lock(mtx);
    ofstream outFile("corrected_text.txt");
    if (!outFile) {
        cerr << "Грешка при отваряне на файла!" << endl;
        return;
    }
    outFile << sharedText;
    outFile.close();
}


void displayText() {
    lock_guard<mutex> lock(mtx);
    cout << "Коригиран текст: " << sharedText << endl;
}

int main() {
    cout << "Въведете текст: ";
    getline(cin, sharedText);

    addToUndo(sharedText);


    thread t1(processText);

    t1.join();


    thread t2(writeTextToFile);

    t2.join();


    thread t3(displayText);

    t3.join();

    return 0;
}
