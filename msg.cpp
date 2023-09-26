#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>

ByteMessage M100(mesSize);  // trk1
ByteMessage M101(mesSize);  // trk2
ByteMessage M102(mesSize);  // trk3
ByteMessage M103(mesSize);  // trk4
ByteMessage M104(mesSize);  // enc0
ByteMessage M105(mesSize);  // enc1
ByteMessage M106(mesSize);  // inc0
ByteMessage M107(mesSize);  // beeper
ByteMessage M108(mesSize);  // green light
ByteMessage M109(mesSize);  // redlight
ByteMessage M110(mesSize);  // amber light
ByteMessage M111(mesSize);  // tubestate
ByteMessage M112(mesSize);  // crawler stste
ByteMessage M113(mesSize);  // voltage
ByteMessage M114(mesSize);  // error 

//ByteMessage* frames[15];
std::vector<ByteMessage*> frames(15);

void printCoreData()
{
    for(int i = 0;i<15;i++)
    {
        std::cout << "Bytemessage: " << (*frames[i]).getName() << "  " << (*frames[i]).copyOfString() << "  String length " << (*frames[i]).len  << std::endl;
    }
}

// device id refers to the crawler or laptop or sbc
std::string formatTrackInstruction(std::string MessageID, std::string deviceID, std::string trackID, int value, std::string direction) 
{
    std::string newMsg = MessageID + "," + deviceID + "," + trackID + "," + formatInt(value,11) + "," + direction;

    return newMsg;
}

std::string formatErrorMessage(std::string MessageID, std::string deviceID, std::string name, char values[], std::string boolActive) 
{
    std::string erstr(values);

     std::string newMsg = MessageID + "," + deviceID + "," + name + "," + erstr + "," + boolActive;

    return newMsg;
}

void initMessages()
{
    std::string M100data = formatTrackInstruction("M100","C","trk1",0,"N");
    M100.LoadString(M100data);

    std::string M101data = formatTrackInstruction("M101","C","trk2",0,"N");
    M101.LoadString(M101data);

    std::string M102data = formatTrackInstruction("M102","C","trk3",0,"N");
    M102.LoadString(M102data);

    std::string M103data = formatTrackInstruction("M103","C","trk4",0,"N");
    M103.LoadString(M103data);

    std::string M104data = formatTrackInstruction("M104","C","enc0",0,"N");
    M104.LoadString(M104data);

    std::string M105data = formatTrackInstruction("M105","C","enc1",0,"N");
    M105.LoadString(M105data);

    std::string M106data = formatTrackInstruction("M106","C","inc0",0,"N");
    M106.LoadString(M106data);

    std::string M107data = formatTrackInstruction("M107","C","beep",0,"N");
    M107.LoadString(M107data);

    std::string M108data = formatTrackInstruction("M108","C","gren",0,"N");
    M108.LoadString(M108data);

    std::string M109data = formatTrackInstruction("M109","C","red0",0,"N");
    M109.LoadString(M109data);

    std::string M110data = formatTrackInstruction("M110","C","ambr",0,"N");
    M110.LoadString(M110data);

    std::string M111data = formatTrackInstruction("M111","C","tube",0,"N");
    M111.LoadString(M111data);

    std::string M112data = formatTrackInstruction("M112","C","crwr",0,"N");
    M112.LoadString(M112data);

    std::string M113data = formatTrackInstruction("M113","C","batt",5623,"N");
    M113.LoadString(M113data);

    char errs[] = "00100000000";
    std::string M114data = formatErrorMessage("M114","C","err0",errs,"N");
    M114.LoadString(M114data);

    frames[0] = &M100;
    frames[1] = &M101;
    frames[2] = &M102;
    frames[3] = &M103;
    frames[4] = &M104;
    frames[5] = &M105;
    frames[6] = &M106;
    frames[7] = &M107;
    frames[8] = &M108;
    frames[9] = &M109;
    frames[10] = &M110;
    frames[11] = &M111;
    frames[12] = &M112;
    frames[13] = &M113;
    frames[14] = &M114;

}

void updateTrackValues()
{
    std::string M100L = M100.copyOfString();
    std::vector<std::string> parts = splitIt(M100L,',');

    std::cout << parts[0] << std::endl;
    std::cout << parts[1] << std::endl;
    std::cout << parts[2] << std::endl;
    std::cout << parts[3] << std::endl;
    std::cout << parts[4] << std::endl;

}

