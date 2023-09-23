#include <iostream>
#include <thread>
#include <atomic>

std::atomic<bool> bEcho(false);


int main(int argc, const char** argv) {

    std::string command;

    std::getline(std::cin, command);

     if (command == "quit" || command == "q" || command == "e")
        {
            
        }else if(command == "data")
        {
            
        }else if(command == "databytes")
        {
            
        }else if(command == "datasize")
        {
            
        }else if(command == "settings")
        {
           
        }else if(command == "echooff")
        {
            bEcho.store(false);
            std::cout << "ECHO OFF" <<  std::endl;
        }else if(command == "echoon")
        {
            bEcho.store(true);
            std::cout << "ECHO ON" <<  std::endl;
        }else if(command == "update")
        {
            
        }


        std::cout << bEcho << std::endl;
    return 0;
}