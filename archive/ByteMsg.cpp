#include <vector>
#include <mutex>
#include <string>

class ByteMessage {
private:
    std::mutex _lock;
    std::vector<char> byteArray;
    

    void emptyArray() {
        std::lock_guard<std::mutex> guard(_lock);
        for (int i = 0; i < byteArray.size(); i++) {
            byteArray[i] = 0;
        }
    }


public:
    int len =0;
    ByteMessage(int s) : len(s) {
        byteArray.resize(len, 0);
    }

    void WriteData(int index, char data) {
        std::lock_guard<std::mutex> guard(_lock);
        byteArray[index] = data;
    }

    std::vector<char> copyOfInternalByteArray() {
        std::lock_guard<std::mutex> guard(_lock);
        std::vector<char> result(len);
        for (int i = 0; i < len; i++) {
            result[i] = byteArray[i];
        }
        return result;
    }

    void LoadString(const std::string& str) {
        emptyArray();
        std::vector<char> bMsg = strToCharVector(str);
        len = str.size();
        for (int i = 0; i < bMsg.size(); i++) {
            WriteData(i, bMsg[i]);
        }
    }
    

    char ReadData(int index) {
        std::lock_guard<std::mutex> guard(_lock);
        return byteArray[index];
    }

    std::string copyOfByteString() {
        std::lock_guard<std::mutex> guard(_lock);
        std::string retval;
        for (int i = 0; i < len; i++) {
            retval += " " + std::to_string(static_cast<int>(byteArray[i]));
        }
        return retval;
    }

    std::string copyOfString() {
        std::lock_guard<std::mutex> guard(_lock);
        std::string retval;
        for (int i = 0; i < len; i++) {
            if(byteArray[i] == '\0')
            {
                
            }else
            {
                retval += byteArray[i];
            }
            
        }
        return retval;
    }

    std::vector<char> strToCharVector(const std::string& msg) {
        return std::vector<char>(msg.begin(), msg.end());
    }

    std::string charVectorToStr(const std::vector<char>& chars) {
        return std::string(chars.begin(), chars.end());
    }
};
