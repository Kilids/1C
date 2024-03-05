#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <chrono>
#include <thread>
#include <set>
#include <string.h>

using namespace std;
using namespace std::chrono;

const int NUM_IP_ADDRESSES = 100;
const int BUFFER_SIZE = 500;
const int MINUTES_TO_WAIT = 2 ; // Изменил на 5 минут

string ip_addresses[NUM_IP_ADDRESSES];

void generateIPAddresses() {
    srand(time(NULL));
    for (int i = 0; i < NUM_IP_ADDRESSES; ++i) {
        ip_addresses[i] = to_string(rand() % 256) + "." + to_string(rand() % 256) + "." + to_string(rand() % 256) + "." + to_string(rand() % 256);
    }
}

string generateRandomIP() {
    return ip_addresses[rand() % NUM_IP_ADDRESSES];
}

void writeDataToFile(const char* buffer, int bufferIndex, ofstream& outputFile) {
    //if (outputFile.is_open()) {
        outputFile.write(buffer, bufferIndex);
        outputFile << "---END---" << endl; // Строка-ограничитель
        cout << "Data written to file successfully." << endl;
    //} 
    //else {
    //    cerr << "Error: Unable to open file for writing." << endl;
    //}
}

void readDataFromFile(char* buffer, int& bufferIndex) {
    ifstream inputFile("user_ip_data.txt");
    if (inputFile.is_open()) {
        string line;
        while (getline(inputFile, line)) {
            if (line == "---END---") {
                break; // Достигнут конец данных
            }
            // Добавьте строку в буфер
            int len = line.length();
            if (bufferIndex + len < BUFFER_SIZE) {
                strcpy(buffer + bufferIndex, line.c_str());
                bufferIndex += len;
                buffer[bufferIndex++] = '\n'; // Добавляем символ новой строки
            } else {
                cerr << "Error: Buffer overflow." << endl;
                break;
            }
        }
        inputFile.close();
        cout << "Data read from file successfully." << endl;
    } else {
        cerr << "Info: File not found. Continuing without reading data from file." << endl;
    }
}

void printBufferContent(const char* buffer, int bufferIndex) {
    cout << "Buffer content:" << endl;
    cout << string(buffer, bufferIndex) << endl;
}

void closeFile(ofstream& outputFile) {
    this_thread::sleep_for(minutes(1)); // Подождать 4 минуты
    outputFile.close(); // Закрыть файл
    cout << "File closed successfully." << endl;
}

int main() {
    generateIPAddresses();
    ofstream outputFile("user_ip_data.txt", ios::app);
    cout << "Server started." << endl;

    char* buffer = new char[BUFFER_SIZE];
    int bufferIndex = 0;

    set<string> uniqueWords;

    auto lastReadTime = steady_clock::now();

    thread fileCloser(closeFile, ref(outputFile)); // Запустить второй поток для закрытия файла через 4 минуты

    do {
        auto currentTime = steady_clock::now();
        auto elapsedMinutes = duration_cast<minutes>(currentTime - lastReadTime).count();

        string userName;
        cout << "Enter user name (or type 'quit' to stop, 'print' to print buffer content): ";
        cin >> userName;

        if (elapsedMinutes >= MINUTES_TO_WAIT) {
            // Прошло 5 минут, читаем данные из файла и заносим их в буфер
            readDataFromFile(buffer, bufferIndex);
            lastReadTime = currentTime; // Обновляем время последнего чтения
        }

        if (userName == "quit") {
            writeDataToFile(buffer, bufferIndex, outputFile);
            delete[] buffer;
            fileCloser.join(); // Дождаться завершения второго потока
            return 0;
        } else if (userName == "print") {
            printBufferContent(buffer, bufferIndex);
        } else {
            // Если прошло 5 минут с момента последнего чтения файла,
            // то данные уже находятся в буфере и мы можем продолжать
            // обработку ввода пользователя без чтения файла
            string ip = generateRandomIP();

            cout << "User: " << userName << " IP: " << ip << endl;

            // Проверяем, является ли слово уникальным в буфере
            if (uniqueWords.find(userName) == uniqueWords.end()) {
                // Если слово уникальное, добавляем его в буфер и в уникальные слова
                int bytesWritten = snprintf(buffer + bufferIndex, BUFFER_SIZE - bufferIndex, "%s:%s\n", userName.c_str(), ip.c_str());
                if (bytesWritten < 0 || bytesWritten >= BUFFER_SIZE - bufferIndex) {
                    cerr << "Error: Insufficient buffer size." << endl;
                    delete[] buffer;
                    return 1;
                }
                bufferIndex += bytesWritten;
                uniqueWords.insert(userName);
            } else {
                // Если слово уже есть в уникальных словах, сообщаем об этом
                cout << "Word \"" << userName << "\" already exists in the buffer." << endl;
            }

            if (bufferIndex >= BUFFER_SIZE || bufferIndex == 0) {
                writeDataToFile(buffer, bufferIndex, outputFile);
                bufferIndex = 0;
                uniqueWords.clear(); // Очищаем уникальные слова после записи в файл
            }
            if (elapsedMinutes >= MINUTES_TO_WAIT) {
                // Прошло 5 минут, читаем данные из файла и заносим их в буфер
                writeDataToFile(buffer, bufferIndex, outputFile);
                lastReadTime = currentTime; // Обновляем время последнего чтения
                delete[] buffer;
                }
        }

    } while (true);

    return 0;
}
