#include "../Bytemessage/ByteMsg.cpp"
#include "../../cpp_csv/csv_cpp.cpp"
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <string.h>
#include <cstdint>
#include <cstring>
#include <chrono>
#include "udp.h"


using boost::asio::ip::udp;

std::mutex mutex;

// remove
std::vector<std::string> messages; // shared data structure


std::string thisip = "";
int sLocalPort = 0;

std::string sRemoteIP = "";

int sRemotePort = 0;
std::string ident = "";



int trackOneSpeed = 0;
int trackTwoSpeed = 0;
int trackThreeSpeed = 0;
int trackFourSpeed = 0;

long enc0Val = 2000;
long enc1Va = 4578;

long tiltAngle = 10;

ByteMessage M100(1024); // Track 1
ByteMessage M101(1024); // Track 2
ByteMessage M102(1024); // Track 3
ByteMessage M103(1024); // Track 4
ByteMessage M104(1024); // enc0
ByteMessage M105(1024); // enc1
ByteMessage M106(1024); // inclinometer

ByteMessage M107(1024); // Crawlers Track 1 setting
ByteMessage M108(1024); // Crawlers Track 2 setting
ByteMessage M109(1024); // Crawlers Track 3 setting
ByteMessage M110(1024); // Crawlers Track 4 setting

ByteMessage M111(1024); // Hold encoder0 value
ByteMessage M112(1024); // Hold encoder1 value
ByteMessage M113(1024); // Hold tilt sensor value




void mainloop()
{
     std::string command;
    // Command line loop to modify the data array
    while (cli_running)
    {
        std::getline(std::cin, command);
        std::cout << command << std::endl;
       
        if (command == "quit" || command == "q" || command == "e")
        {
            std::cout << "quit " << std::endl;
            sender_running = false;
            receiver_running = false;
            tcp_sender_running = false;
            tcp_receiver_running = false;
            cli_running = false;
            send_quit();
            break;
        }else if (command == "get")
        {
            
            std::string tMsg = M100.copyOfString();
            std::cout << tMsg << std::endl;
            command = "";
            //std::cout << index << std::endl;
        }
    }
    return;
}


void send_thread_func()
{
    try
    {
        boost::asio::io_context io_context;

        udp::resolver resolver(io_context);
        udp::endpoint broadcast_endpoint = *resolver.resolve(udp::v4(), "255.255.255.255", sRemotePort).begin();

        udp::socket socket(io_context);
        socket.open(udp::v4());

        udp::endpoint local_endpoint(boost::asio::ip::address::from_string(thisip), 0); // replace "192.168.1.5" with your desired local IP address
        socket.bind(local_endpoint);

        // Set the socket option for broadcasting
        socket.set_option(boost::asio::socket_base::broadcast(true));
        
        int64_t lp = (int64_t)loop_speed;

        // send broadcast message in a loop
        while (sender_running)
        {
            // Send track 1 data
            //std::string message = M101.copyOfString();
            std::string message = M100.copyOfString();
            std::array<char, 1024> send_buffer;
            std::copy(message.begin(), message.end(), send_buffer.begin());
            socket.send_to(boost::asio::buffer(send_buffer), broadcast_endpoint);

            // Send track 2 data
            std::string message2 = M101.copyOfString();
            std::array<char, 1024> send_buffer2;
            std::copy(message2.begin(), message2.end(), send_buffer2.begin());
            socket.send_to(boost::asio::buffer(send_buffer2), broadcast_endpoint);

            // Send track 3 data
            std::string message3 = M102.copyOfString();
            std::array<char, 1024> send_buffer3;
            std::copy(message3.begin(), message3.end(), send_buffer3.begin());
            socket.send_to(boost::asio::buffer(send_buffer3), broadcast_endpoint);

            // Send track 4 data
            std::string message4 = M103.copyOfString();
            std::array<char, 1024> send_buffer4;
            std::copy(message4.begin(), message4.end(), send_buffer4.begin());
            socket.send_to(boost::asio::buffer(send_buffer4), broadcast_endpoint);

            // Send track 4 data
            std::string message5 = M104.copyOfString();
            std::array<char, 1024> send_buffer5;
            std::copy(message5.begin(), message5.end(), send_buffer5.begin());
            socket.send_to(boost::asio::buffer(send_buffer5), broadcast_endpoint);

            // Send track 4 data
            std::string message6 = M105.copyOfString();
            std::array<char, 1024> send_buffer6;
            std::copy(message6.begin(), message6.end(), send_buffer6.begin());
            socket.send_to(boost::asio::buffer(send_buffer6), broadcast_endpoint);

            // Send track 4 data
            std::string message7 = M106.copyOfString();
            std::array<char, 1024> send_buffer7;
            std::copy(message7.begin(), message7.end(), send_buffer7.begin());
            socket.send_to(boost::asio::buffer(send_buffer7), broadcast_endpoint);

            std::this_thread::sleep_for(std::chrono::milliseconds(lp)); // wait for 1 second before sending next message
            
          
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
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
        std::array<char, 20> receive_buffer;
        udp::endpoint sender_endpoint;

        // receive broadcast message in a loop
        while (receiver_running)
        {
           
            receive_socket.receive_from(boost::asio::buffer(receive_buffer), sender_endpoint);
            // print received message
            std::string str = std::string(receive_buffer.data(), receive_buffer.size());
            std::cout << str << std::endl;
           
            std::lock_guard<std::mutex> lock(mutex);
            messages.push_back(std::string(receive_buffer.data(), receive_buffer.size()));
            
            if((str.substr(1, 3) == "trk" && str.size() > 12) || (str.substr(0, 2) == "trk" && str.size() > 12))
            {
                updateValues(str);
            }else{
                if(str.substr(1,3) == "inc" || str.substr(1,3) == "enc" || str == "uit")
                {

                }else
                {
                    std::cout << "Incorrect format for tracks: "<< str.substr(1,3) << std::endl;
                }
                
            }
            

            if (messages.size() > 10)
            {
               messages.clear();
            }
            
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}




int main(int argc, char* argv[])
{

    
    std::string M100data = "2trk1,00000,00000,N";
    std::string M101data = "2trk2,00000,00000,N";
    std::string M102data = "2trk3,00000,00000,N";
    std::string M103data = "2trk4,00000,00000,N";
    std::string M104data = "2enc0,00000,00000,N";
    std::string M105data = "2enc1,00000,00000,N";
    std::string M106data = "2inc0,00000,00000,N";

    M100.LoadString(M100data);
    M101.LoadString(M101data);
    M102.LoadString(M102data);
    M103.LoadString(M103data);
    M104.LoadString(M104data);
    M105.LoadString(M105data);
    M106.LoadString(M106data);

    std::string M107data = "ctrk1,00000,00000,N";
    std::string M108data = "ctrk2,00000,00000,N";
    std::string M109data = "ctrk3,00000,00000,N";
    std::string M110data = "ctrk4,00000,00000,N";


    M107.LoadString(M107data);
    M108.LoadString(M108data);
    M109.LoadString(M109data);
    M110.LoadString(M110data);

    std::string M111data = "cenc0,00000,00000,N";
    std::string M112data = "cenc1,00000,00000,N";
    std::string M113data = "cinc0,00000,00000,N";

    M111.LoadString(M111data);
    M112.LoadString(M112data);
    M113.LoadString(M113data);



    //std::cout << M100.copyOfString() << std::endl;

    thisip = argv[1];
    port_no = atoi(argv[2]);
    sLocalPort = argv[2];


    remote_port = atoi(argv[3]);
    sRemotePort = argv[3];
    loop_speed = atoi(argv[4]);
    
    ident = argv[5];
    std::cout << thisip << std::endl;

    std::cout << port_no << std::endl;
    std::cout << remote_port << std::endl;
    std::cout << loop_speed << std::endl;

    // create threads for sending and receiving broadcasts
    std::thread send_thread(send_thread_func);
    std::thread receive_thread(receive_thread_func);
    std::thread mnloop(mainloop);

    std::thread tcp_send_thread(tcpSendLoop);
    std::thread tcp_receive_thread(tcpReceiveLoop);


    // wait for threads to finish
    send_thread.join();
    receive_thread.join();

    tcp_send_thread.join();
    tcp_receive_thread.join();

    mnloop.join();


    return 0;
}


void updateValues(std::string newMsg)
{
    // Split CSV string into separate values
    std::vector<std::string> values = splitIt(newMsg, ',');

     // Convert the second and third values to integers
    try {
        int value2 = std::stoi(values[1]);
        int value3 = std::stoi(values[2]);


        std::cout << "newMsg " << newMsg << std::endl;
        std::cout << "Second value as integer: " << value2 << std::endl;
        std::cout << "Third value as integer: " << value3 << std::endl;


        if(values[0] == "trk1" || values[0] == "2trk1")
        {
            if(trackOneSpeed != value2)
            {
                trackOneSpeed = value2;
                std::string M107data = "2trk1," + values[1] + ","+ values[2] + ",N";
                M107.LoadString(M107data);
                M100.LoadString(M107data);
            }
        }else if(values[0] == "trk2" || values[0] == "2trk2")
        {
            if(trackTwoSpeed != value2)
            {
                trackTwoSpeed = value2;
                std::string M108data = "2trk2," + values[1] + ","+ values[2] + ",N";
                M108.LoadString(M108data);
                M101.LoadString(M108data);
            }
        }else if(values[0] == "trk3" || values[0] == "2trk3")
        {
            if(trackThreeSpeed != value2)
            {
                trackThreeSpeed = value2;
                std::string M109data = "2trk3," + values[1] + ","+ values[2] + ",N";
                M109.LoadString(M109data);
                M102.LoadString(M109data);
            }
        }else if(values[0] == "trk4" || values[0] == "2trk4")
        {
            if(trackFourSpeed != value2)
            {
                trackFourSpeed = value2;
                std::string M110data = "2trk4," + values[1] + ","+ values[2] + ",N";
                M110.LoadString(M110data);
                M103.LoadString(M110data);
            }
        }


    } catch (const std::exception &e) {
        std::cerr << "Error converting string to integer: " << e.what() << std::endl;
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

        std::string sUE = "quit";
        std::string message = sUE;
        std::array<char, 4> send_buffer;
        std::copy(message.begin(), message.end(), send_buffer.begin());

         socket.send_to(boost::asio::buffer(send_buffer), broadcast_endpoint);
}


void tcpSendLoop()
{
    while(tcp_sender_running)
    {
        std::cout << "TCP sender" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // wait for 1 second before sending next message
    }
    
}
void tcpReceiveLoop()
{
    while(tcp_receiver_running)
    {
        std::cout << "TCP receiver" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // wait for 1 second before sending next message
    }
}

