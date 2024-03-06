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
const int MINUTES_TO_WAIT = 2; // Изменил на 5 минут

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
    if (outputFile.is_open()) {
        outputFile.write(buffer, bufferIndex);
        outputFile << "---END---" << endl; // Строка-ограничитель
        cout << "Data written to file successfully." << endl;
        outputFile.close();
    } else {
        cerr << "Error: Unable to open file for writing." << endl;
    }
}

void readDataFromFile(char* buffer, int& bufferIndex, int startTagCount, int endTagCount) {
    ifstream inputFile("C:\\Users\\kilid\\test2\\user_ip_data.txt"); // Открываем файл для чтения
    if (inputFile.is_open()) { // Проверяем, удалось ли открыть файл
        string line; // Переменная для хранения считанной строки из файла
        while (getline(inputFile, line)) { // Читаем файл построчно
            if (line == "---END---") { // Если встретили тег "---END---"
                startTagCount++; // Увеличиваем счет конец интервала
            } else if (startTagCount == (endTagCount - 1)) { // Если встретили строку между предпоследним и последним тегами "---END---"
                // Добавляем строку в буфер
                int len = line.length(); // Вычисляем длину считанной строки
                if (bufferIndex + len < BUFFER_SIZE) { // Проверяем, поместится ли строка в буфер
                    strcpy(buffer + bufferIndex, line.c_str()); // Копируем считанную строку в буфер
                    bufferIndex += len; // Увеличиваем индекс буфера на длину считанной строки
                    buffer[bufferIndex++] = '\n'; // Добавляем символ новой строки
                } else {
                    cerr << "Error: Buffer overflow." << endl; // Сообщаем об ошибке переполнения буфера
                    break; // Прекращаем чтение из-за переполнения буфера
                }
            }
        }
        inputFile.close(); // Закрываем файл после завершения чтения
        cout << "Data read from file successfully." << endl; // Выводим сообщение об успешном чтении данных
    } else {
        cerr << "Info: File not found. Continuing without reading data from file." << endl; // Сообщаем, что файл не найден
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

bool isValidUserName(const string& userName) {
    for (char c : userName) {
        if (!isalpha(c) || !islower(c)) {
            return false;
        }
    }
    return true;
}

int main() {
    generateIPAddresses();
    ofstream outputFile("C:\\Users\\kilid\\test2\\user_ip_data.txt", ios::app);
    cout << "Server started." << endl;
    int reservIndex;
    char* buffer = new char[BUFFER_SIZE];
    int bufferIndex = 0;
    int endTagCount = 0; // Счетчик для отслеживания количества встреченных тегов "---END---"
    int startTagCount = 0; // Первая точка для диапозона счетчика  "---END---"
    set<string> uniqueWords;

    auto lastReadTime = steady_clock::now();

    do {
        auto currentTime = steady_clock::now();
        auto elapsedMinutes = duration_cast<minutes>(currentTime - lastReadTime).count();

        string userName;
        cout << "Enter user name (or type 'quit' to stop, 'print' to print buffer content, 'del' to delete a user): ";
        cin >> userName;
        if (userName == "quit") {
            ofstream outputFile("C:\\Users\\kilid\\test2\\user_ip_data.txt", ios::app);
            writeDataToFile(buffer, bufferIndex, outputFile);
            delete[] buffer;
            return 0;
        } else if (userName == "print") {
            printBufferContent(buffer, bufferIndex);
        } else if (userName == "del") {
            cout << "Enter the username you want to delete: ";
            string delUserName;
            cin >> delUserName;
            uniqueWords.erase(delUserName); // Удаляем пользователя из уникальных слов
            // Теперь перезаписываем буфер без удаленного пользователя
            int shift = 0; // Переменная для хранения смещения
            for (int i = 0; i < bufferIndex; ++i) {
                if (buffer[i] == '\n') {
                    string line(buffer + shift, buffer + i);
                    string user = line.substr(0, line.find(':'));
                    if (user != delUserName) {
                        int len = line.length(); 
                        shift += len + 1;
                    }
                    if (user == delUserName) {
                        // Если найден пользователь, который нужно удалить, смещаем оставшуюся часть буфера влево
                        int len = i - shift + 1; // Длина строки, включая символ новой строки
                        memmove(buffer + shift, buffer + i + 1, bufferIndex - i); // Смещаем оставшуюся часть буфера влево
                        bufferIndex -= len; // Уменьшаем индекс буфера на длину удаленной строки
                        shift = i; // Обновляем смещение
                    }
                }
            }
            bufferIndex = shift;
            cout << "User \"" << delUserName << "\" deleted successfully." << endl;
        } else {
            // Проверка на валидность имени пользователя
            if (!isValidUserName(userName)) {
                auto asyncCloseFile = async(launch::async, closeFile, ref(outputFile)); // Запустить второй поток для закрытия файла через 4 минуты
                cout << "Warning: The user name must consist of lowercase Latin letters only. Please enter a valid name." << endl;
                continue;
            }

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
                    memset(buffer, 0, BUFFER_SIZE);
                    return 1;
                }
                bufferIndex += bytesWritten;
                uniqueWords.insert(userName);
            } else {
                // Если слово уже есть в уникальных словах, сообщаем об этом
                cout << "Word \"" << userName << "\" already exists in the buffer." << endl;
            }

            if (bufferIndex >= BUFFER_SIZE) {
                ofstream outputFile("C:\\Users\\kilid\\test2\\user_ip_data.txt", ios::app);
                writeDataToFile(buffer, bufferIndex, outputFile);
                memset(buffer, 0, BUFFER_SIZE);
                uniqueWords.clear(); // Очищаем уникальные слова после записи в файл
            }
            if (elapsedMinutes >= MINUTES_TO_WAIT) {
                ofstream outputFile("C:\\Users\\kilid\\test2\\user_ip_data.txt", ios::app);
                // Прошло 5 минут, читаем данные из файла и заносим их в буфер
                writeDataToFile(buffer, bufferIndex, outputFile);
                lastReadTime = currentTime; // Обновляем время последнего чтения
                // Полностью очищаем память, заполнив буфер нулями
                reservIndex = bufferIndex;
                bufferIndex = 0;
                memset(buffer, 0, BUFFER_SIZE);
                endTagCount++; // Увеличиваем счет начала интервала
            }
            if (elapsedMinutes >= MINUTES_TO_WAIT) {
                // Прошло 5 минут, читаем данные из файла и заносим их в буфер
                readDataFromFile(buffer, bufferIndex, startTagCount, endTagCount);
                lastReadTime = currentTime; // Обновляем время последнего чтения
                bufferIndex = reservIndex;
            }
        }
    } while (true);

    return 0;
}
