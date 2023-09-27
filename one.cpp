#include <vector>
#include "udp.h"
#include "ByteMsgv2.cpp"
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <string.h>
#include <cstdint>
#include <cstring>
#include <chrono>
#include "msg.cpp"

using boost::asio::ip::udp;

const int ByteMessageSise = 25;
int loop_speed = 20;
std::atomic<bool> receiver_running(true);
std::atomic<bool> cli_running(true);
std::atomic<bool> sender_running(true);
std::atomic<bool> heart_beat_running(true);

std::atomic<bool> bEcho(false);


std::mutex mutex;
std::string sessionLabel = "default";
std::string thisip = "192.168.1.30";
std::string remoteip = "255.255.255.255";
std::string sLocalPort = "12345";
std::string sRemotePort = "12346";

// test from sbc

int port_no = 12345;

int64_t heartbeatloopspeed = 1500;
int toggleVale = 1;
bool laptopconnected = false;
std::vector<std::string> messages; // shared data structure

ByteMessage Data(ByteMessageSise);
ByteMessage heartbeatMutex(1);

int main(int argc, char* argv[])
{
        initMessages();

        if(argc > 5)
    {
        thisip = argv[1];
        sLocalPort = argv[2];
        remoteip = argv[3];
        sRemotePort = argv[4];
        port_no = atoi(sLocalPort.c_str());
        sessionLabel = argv[5];
    }else
    {
        std::cout << "Ip Settings set to default." << std::endl;
    }

    std::cout << "Ip Address " <<  thisip << std::endl;
    std::cout << "Port" <<  sLocalPort << std::endl;
    std::cout << "Target Ip Address " <<  remoteip << std::endl;
    std::cout << "Target Port " <<  sRemotePort << std::endl;
    
    Data.LoadString("");
    heartbeatMutex.LoadString("");
   // std::thread send_thread(send_thread_func2);
    std::thread receive_thread(receive_thread_func);
    std::thread mnloop(mainloop);
    std::thread heart_beat(heartbeat);

    // send_thread.join();
    receive_thread.join();
    mnloop.join();
    heart_beat.join();
    
    //return;
        //launchPad(argc,argv);


    
    return 0;
}



void heartbeat()
{
    
    while(heart_beat_running)
    {
        //std::cout << "heartbeat ";
        bool bFound = false;
        for(int i = 0;i<10;i++)
        {
            if(heartbeatMutex.copyOfString() == "L")
            {
                bFound = true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds((int64_t)40));
        }
        if(bFound && !laptopconnected)
        {
            laptopconnected = true;
            std::cout << "Control link established" << std::endl;
        }else if(laptopconnected && !bFound)
        {
            std::cout << "Control link lost, Putting into safe state." << std::endl;
            laptopconnected = false;
        }
        //std::cout << heartbeatMutex.copyOfString() << std::endl;
        heartbeatMutex.LoadString("");
        std::this_thread::sleep_for(std::chrono::milliseconds(heartbeatloopspeed)); // wait for 1 second before sending next message
    }
}


void mainloop()
{
     std::string command;
     
    // Command line loop to modify the data array
    while (cli_running)
    {
        std::getline(std::cin, command);
        //std::cout << command << std::endl;
       
        if (command == "quit" || command == "q" || command == "e")
        {
            std::cout << "quit " << std::endl;
            sender_running.store(false);
            receiver_running.store(false);
            cli_running.store(false);
            heart_beat_running.store(false);
            send_quit();
            clearTerm();
            std::this_thread::sleep_for(std::chrono::milliseconds((int64_t)5));
            std::cout << "Bye!!\n" << std::endl;
            break;
        }else if(command == "data")
        {
            command = "";
            printCoreData();
        }else if(command == "databytes")
        {
            command = "";
            PrintDataBytes();
        }else if(command == "datasize")
        {
            command = "";
            PrintDataSize();
        }else if(command == "settings")
        {
            command = "";
            std::cout << "Ip Address " <<  thisip << std::endl;
            std::cout << "Port" <<  sLocalPort << std::endl;
            std::cout << "Target Ip Address " <<  remoteip << std::endl;
            std::cout << "Target Port " <<  sRemotePort << std::endl;
        }else if(command == "ef")
        {
            bEcho.store(false);
            std::cout << "ECHO OFF" <<  std::endl;
        }else if(command == "eo")
        {
            bEcho.store(true);
            std::cout << "ECHO ON" <<  std::endl;
        }else if(command == "update")
        {
            updateTrackValues();
        }else if(command == "cls")
        {
            clearTerm();
        }
        else
        {
            send_thread_func(command,remoteip);
            command = "";
            std::cout << "Data sent\n" << std::endl;
        }
    }
    return;
}

void printHelp()
{
    std::cout << "ef = echo off" <<  std::endl;
    std::cout << "eo = echo on" <<  std::endl;
    std::cout << "cls = clear terminal" <<  std::endl;
    std::cout << "settings = ip addresses and ports" << std::endl;
    std::cout << "databytes print byte string" << std::endl;
    std::cout << "data = print data structure" << std::endl;
}

void send_thread_func(std::string msg, std::string tempip)
{
    
        boost::asio::io_context io_context;

        udp::resolver resolver(io_context);
        udp::endpoint broadcast_endpoint = *resolver.resolve(udp::v4(), tempip, sRemotePort).begin();

        udp::socket socket(io_context);
        socket.open(udp::v4());
        // Set the socket option for broadcasting
        socket.set_option(boost::asio::socket_base::broadcast(true));
        socket.set_option(boost::asio::socket_base::reuse_address(true));
        udp::endpoint local_endpoint(boost::asio::ip::address::from_string(thisip), 0); 
        socket.bind(local_endpoint);

        std::string message = msg;
        std::array<char, 1024> send_buffer;
        std::copy(message.begin(), message.end(), send_buffer.begin());
        socket.send_to(boost::asio::buffer(send_buffer), broadcast_endpoint);
        message = "";
        for (char& c : send_buffer) 
        {
            c = '\0';
        }
}

void send_thread_func2()
{
    
        boost::asio::io_context io_context;

        udp::resolver resolver(io_context);
        udp::endpoint broadcast_endpoint = *resolver.resolve(udp::v4(), remoteip, sRemotePort).begin();

        udp::socket socket(io_context);
        socket.open(udp::v4());
        // Set the socket option for broadcasting
        socket.set_option(boost::asio::socket_base::broadcast(true));
        socket.set_option(boost::asio::socket_base::reuse_address(true));
        udp::endpoint local_endpoint(boost::asio::ip::address::from_string(thisip), 0); 
        socket.bind(local_endpoint);
    while(sender_running)
    {
        std::string message = "";
        for(int i = 0;i<15;i++)
        {
            std::string message = frames[i]->copyOfString();
            std::array<char, 1024> send_buffer;
            std::copy(message.begin(), message.end(), send_buffer.begin());
            socket.send_to(boost::asio::buffer(send_buffer), broadcast_endpoint);
            message = "";
            std::this_thread::sleep_for(std::chrono::milliseconds((int64_t)9)); // wait for 1 second before sending next message
        }
        //
        // std::array<char, 1024> send_buffer;
        // std::copy(message.begin(), message.end(), send_buffer.begin());
        // socket.send_to(boost::asio::buffer(send_buffer), broadcast_endpoint);
        message = "";
        // for (char& c : send_buffer) 
        // {
        //     c = '\0';
        // }
        std::this_thread::sleep_for(std::chrono::milliseconds((int64_t)loop_speed)); // wait for 1 second before sending next message
    }
}

void receive_thread_func()
{
    int messagect = 0;
    try
    {
        boost::asio::io_context io_context;
        // create socket for receiving broadcasts
        udp::socket receive_socket(io_context, udp::endpoint(udp::v4(), (unsigned short)port_no));

        // create buffer to receive data
        std::array<char, ByteMessageSise + 1024> receive_buffer;
        udp::endpoint sender_endpoint;

        // receive broadcast message in a loop
        while (receiver_running)
        {
            //Data.LoadString("");
            receive_socket.receive_from(boost::asio::buffer(receive_buffer), sender_endpoint);
            // print received message
            std::string str = std::string(receive_buffer.data(), receive_buffer.size());
            
            if(bEcho)
            {
                std::cout << str << std::endl;
            }
            if(str.substr(0,3) == "quit" || str[0] == 'q' || str[0] == 'e')
            {
                break;
            }
            Data.LoadString(str.substr(0, ByteMessageSise -1));
            
            for (char& c : receive_buffer) 
            {
                c = '\0';
            }
            
            if(str.substr(0,2) == "M1")
            {
                std::vector<std::string> parts = splitIt(Data.copyOfString(),',');
                if(parts[1] == "L")
                {
                    heartbeatMutex.LoadString(parts[1]);
                }else
                {
                    heartbeatMutex.LoadString("");
                }
            }else
            {
                std::cout << str.substr(0,1) << std::endl;
            }
            str = "";
            messagect++;
            if(messagect > 200 && bEcho)
            {
                clearTerm();
                messagect = 0;
            }
            //updatefromremote(parts);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void send_quit()
{
        boost::asio::io_context io_context;

        udp::resolver resolver(io_context);
        udp::endpoint broadcast_endpoint = *resolver.resolve(udp::v4(), thisip, sLocalPort).begin();
        udp::socket socket(io_context);
        socket.open(udp::v4());
        // Set the socket option for broadcasting
        socket.set_option(boost::asio::socket_base::broadcast(true));
        socket.set_option(boost::asio::socket_base::reuse_address(true));
        std::string sUE = "quit";
        std::string message = sUE;
        std::array<char, 1024> send_buffer;
        std::copy(message.begin(), message.end(), send_buffer.begin());

        socket.send_to(boost::asio::buffer(send_buffer), broadcast_endpoint);
}

void PrintData()
{
    if(Data.len > 0)
    {
        std::cout << Data.copyOfString() << std::endl;
    }else
    {
        std::cout << "null" << std::endl;
    }
}

void PrintDataBytes()
{
    std::cout << Data.copyOfByteString() << std::endl;
}

void PrintDataSize()
{
    int sz = 0;
    std::string temp = Data.copyOfString();
    
    std::cout << Data.Size() << " bytes" << std::endl;
}