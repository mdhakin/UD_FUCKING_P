#include <iomanip>
#include <sstream>



const int mesSize = 25;
void heartbeat();
void mainloop();
void send_thread_func(std::string msg, std::string tempip);
void receive_thread_func();
void send_quit();
void PrintData();
void PrintDataBytes();
void PrintDataSize();

void printCoreData();

std::vector<std::string> splitIt(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    size_t pos = 0;
    size_t end_pos = s.find(delimiter);

    while (end_pos != std::string::npos) {
        token = s.substr(pos, end_pos - pos);
        tokens.push_back(token);
        pos = end_pos + 1;
        end_pos = s.find(delimiter, pos);
    }
    tokens.push_back(s.substr(pos));

    return tokens;
}

std::string formatInt(int value, int width = 5) {
    std::ostringstream oss;
    oss << std::setw(width) << std::setfill('0') << value;
    return oss.str();
}


