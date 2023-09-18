#include "ByteMsgv2.cpp"
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <string.h>
#include <cstdint>
#include <cstring>
#include <chrono>

#include "msg.cpp"

using boost::asio::ip::udp;

const int ByteMessageSise = 25;

std::atomic<bool> receiver_running(true);
std::atomic<bool> cli_running(true);
std::mutex mutex;
std::string sessionLabel = "default";
std::string thisip = "192.168.1.30";
std::string remoteip = "255.255.255.255";
std::string sLocalPort = "12345";
std::string sRemotePort = "12346";
int port_no = 12345;
int loop_speed = 20;

std::vector<std::string> messages; // shared data structure

ByteMessage Data(ByteMessageSise);



int main(int argc, char* argv[])
{
    
        initMessages();
        launchPad(argc,argv);
    //
    
    return 0;
}


void launchPad(int argc, char* argv[])
{
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
    //std::thread send_thread(send_thread_func);
    Data.LoadString("");

    std::thread receive_thread(receive_thread_func);
    std::thread mnloop(mainloop);

    receive_thread.join();
    mnloop.join();
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
            receiver_running = false;
            cli_running = false;
            send_quit();
            break;
        }else if(command == "data")
        {
            command = "";
            PrintData();
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
        udp::endpoint local_endpoint(boost::asio::ip::address::from_string(thisip), 0); // replace "192.168.1.5" with your desired local IP address
        socket.bind(local_endpoint);

        
        std::string message = msg;
        std::array<char, 25> send_buffer;
        std::copy(message.begin(), message.end(), send_buffer.begin());
        socket.send_to(boost::asio::buffer(send_buffer), broadcast_endpoint);
        message = "";
        for (char& c : send_buffer) 
        {
            c = '\0';
        }
}


void receive_thread_func()
{
    try
    {
        boost::asio::io_context io_context;

        // create socket for receiving broadcasts
        udp::socket receive_socket(io_context, udp::endpoint(udp::v4(), (unsigned short)port_no));

        // create buffer to receive data
        std::array<char, 25> receive_buffer;
        udp::endpoint sender_endpoint;

        // receive broadcast message in a loop
        while (receiver_running)
        {
            //Data.LoadString("");
            receive_socket.receive_from(boost::asio::buffer(receive_buffer), sender_endpoint);
            // print received message
            std::string str = std::string(receive_buffer.data(), receive_buffer.size());
            std::cout << str << std::endl;
            
            
            Data.LoadString(str);
            
            for (char& c : receive_buffer) 
            {
                c = '\0';
            }
            str = "";
            
            
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
        udp::endpoint broadcast_endpoint = *resolver.resolve(udp::v4(), "255.255.255.255", sLocalPort).begin();

        udp::socket socket(io_context);
        socket.open(udp::v4());

        // Set the socket option for broadcasting
        socket.set_option(boost::asio::socket_base::broadcast(true));
        socket.set_option(boost::asio::socket_base::reuse_address(true));
        std::string sUE = "quit";
        std::string message = sUE;
        std::array<char, 4> send_buffer;
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