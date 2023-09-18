#include <vector>
#include <mutex>
#include <string>
#include "udp.h"

class ByteMessage {
private:
    std::mutex _lock;
    std::vector<char> byteArray;
    std::string sID;
    std::string nodeID;
    std::string deviceCode;
    std::string payload;
    std::string modbit;

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

    std::string getName()
    {
        return sID;
    }
    ByteMessage() {
        len = 25;
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
        std::vector<std::string> splitit1 = splitIt(copyOfString(),',');
        
        if(splitit1.size() == 5)
        {
            sID = splitit1[0];
            nodeID = splitit1[1];
            deviceCode = splitit1[2];
            payload = splitit1[3];
            modbit = splitit1[4];
        }else
        {
            sID = "null";
            nodeID = "null";
            deviceCode = "null";
            payload = "null";
            modbit = "null";
        }
        
    }
    
    std::size_t Size()
    {
        std::string temp = copyOfString();
        return temp.size();
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
