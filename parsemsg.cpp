#include "parsemsg.h"

ParseMsg::ParseMsg(std::string* msgfile)
{
    msgfilepath = msgfile;
    //std::cout << "msgfile: " << *msgfilepath << std::endl;
}

bool ParseMsg::VerifyHeader()
{
    bool ismsg = false;
    uint64_t msghdr = 0;
    uint8_t* tmp8 = new uint8_t[8];
    std::ifstream msgstream;
    msgstream.open(msgfilepath->c_str(), std::ios::in|std::ios::binary);
    msgstream.seekg(0);
    msgstream.read((char*)tmp8, 8);
    msgstream.close();
    msghdr = (uint64_t)tmp8[0] | (uint64_t)tmp8[1] << 8 | (uint64_t)tmp8[2] << 16 | (uint64_t)tmp8[3] << 24 | (uint64_t)tmp8[4] << 32 | (uint64_t)tmp8[5] << 40 | (uint64_t)tmp8[6] << 48 | (uint64_t)tmp8[7] << 56;
    if(msghdr == 0xe11ab1a1e011cfd0)
        ismsg = true;

    return ismsg;
}
