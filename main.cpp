#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <chrono>
#include <thread>
#include <set>
#include <vector>
#include <string.h>
#include <cctype>
#include <future>

using namespace std;
using namespace std::chrono;

const int NUM_IP_ADDRESSES = 100;
const int BUFFER_SIZE = 5000;
const int MINUTES_TO_WAIT = 1;

vector<string> ip_addresses;
unique_ptr<vector<string>> buffer;

void generateIPAddresses() {
    srand(time(NULL));
    for (int i = 0; i < NUM_IP_ADDRESSES; ++i) {
        ip_addresses.push_back(to_string(rand() % 256) + "." + to_string(rand() % 256) + "." +
                            to_string(rand() % 256) + "." + to_string(rand() % 256));
    }
}

string generateRandomIP() {
    return ip_addresses[rand() % NUM_IP_ADDRESSES];
}

void writeDataToFile(const vector<string>& buffer, ofstream& outputFile, int& error, int& endTagCount) {
    if (outputFile.is_open()) {
        for (const string& line : buffer) {
            outputFile << line << endl;
        }
    }
    if (!outputFile.is_open()) {
        error = 1;
        return;
    }
    endTagCount++;
    cout << "Data written to file successfully." << endl;
}
void readDataFromFile(unique_ptr<vector<string>>& buffer, int endTagCount,int reservIndex) {
    ifstream inputFile("C:\\Users\\kilid\\test2\\user_ip_data.txt");
    int startTagCount = 0;
    if (inputFile.is_open()) {
        string line;
        while (getline(inputFile, line)) {
            if (line == "---END---") {
                startTagCount++;
            } 
            if (startTagCount == (endTagCount - 1) && line != "---END---") {
                buffer->push_back(line);
            }
        }
        inputFile.close();
        cout << "Data read from file successfully." << endl;
    } else {
        cerr << "Info: File not found. Continuing without reading data from file." << endl;
    }
}

void printBufferContent(const vector<string>& buffer) {
    cout << "Buffer content:" << endl;
    for (const string& line : buffer) {
        cout << line << endl;
    }
}

void closeFile(ofstream& outputFile) {
    outputFile.close();
}

bool isValidUserName(const string& userName) {
    for (char c : userName) {
        if (!isalnum(c) && c != '_') {
            return false;
        }
        if (iswalpha(c) && !iswalpha(static_cast<wchar_t>(c))) {
            return false;
        }
    }
    return true;
}

int main() {
    generateIPAddresses();
    ofstream outputFile("C:\\Users\\kilid\\test2\\user_ip_data.txt", ios::app);
    cout << "Server started." << endl;
    int error = 0;
    int reservIndex;
    int endTagCount = 0;
    set<string> uniqueWords;
    auto lastReadTime = steady_clock::now();

    buffer = make_unique<vector<string>>();

    do {
        auto currentTime = steady_clock::now();
        auto elapsedMinutes = duration_cast<minutes>(currentTime - lastReadTime).count();

        string userName;
        cout << "Enter user name (or type 'quit' to stop, 'print' to print buffer content, 'del' to delete a user): ";
        cin >> userName;
        if (userName == "quit") {
            writeDataToFile(*buffer, outputFile, error, endTagCount);
            return 0;
        } else if (userName == "print") {
            printBufferContent(*buffer);
        } else if (userName == "del") {
            cout << "Enter the username you want to delete: ";
            string delUserName;
            cin >> delUserName;

            auto it = uniqueWords.find(delUserName);
            if (it != uniqueWords.end()) {
                uniqueWords.erase(it);

                for (int i = buffer->size() - 1; i >= 0; --i) {
                    if (buffer->at(i).find(delUserName) != string::npos) {
                        buffer->erase(buffer->begin() + i);
                    }
                }

                cout << "User \"" << delUserName << "\" deleted successfully." << endl;
            } else {
                cout << "User \"" << delUserName << "\" not found." << endl;
            }
        } else {
            if (!isValidUserName(userName)) {
                auto asyncCloseFile = async(launch::async, closeFile, ref(outputFile));
                cout << "Warning: The user name must consist of Latin letters and numbers only. Please enter a valid name." << endl;
                continue;
            }

            string ip = generateRandomIP();

            cout << "User: " << userName << " IP: " << ip << endl;

            if (uniqueWords.find(userName) == uniqueWords.end()) {
                buffer->push_back(userName + ":" + ip);
                uniqueWords.insert(userName);
            } else {
                cout << "Username \"" << userName << "\" already exists in the buffer." << endl;
            }

            if (buffer->size() >= BUFFER_SIZE) {
                writeDataToFile(*buffer, outputFile,error,endTagCount);
                buffer->clear();
                uniqueWords.clear();
            }

            if (error == 1) {
                outputFile.open("C:\\Users\\kilid\\test2\\user_ip_data.txt", ios::app);
                if (!outputFile.is_open()) {
                    cerr << "Error: Unable to open file for writing." << endl;
                } else {
                    error = 0;
                    }
            }

            if (elapsedMinutes >= MINUTES_TO_WAIT) {
                writeDataToFile(*buffer, outputFile,error,endTagCount);
                lastReadTime = currentTime;
                if (!buffer->empty()) {
                    outputFile << "---END---" << endl;
                }
                reservIndex = buffer->size();
                buffer->clear();

                readDataFromFile(buffer, endTagCount, reservIndex);
                lastReadTime = currentTime;
            }
        }
    } while (true);

    return 0;
}
