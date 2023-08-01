#include "parsemsg.h"

ParseMsg::ParseMsg(std::string* msgfile)
{
    // PARSE HEADER SECTOR HERE
    msgfilepath = msgfile;
    //std::cout << "msgfile: " << *msgfilepath << std::endl;
}

bool ParseMsg::VerifyHeader()
{
    bool ismsg = false;
    uint64_t msghdr = 0;
    uint8_t* tmp8 = new uint8_t[8];
    ReadContent(tmp8, 0, 8);
    ReturnUint64(&msghdr, tmp8);
    if(msghdr == 0xe11ab1a1e011cfd0)
        ismsg = true;

    return ismsg;
}

void ParseMsg::ReadContent(uint8_t* buf, uint64_t pos, uint64_t size)
{
    msgbuffer.open(msgfilepath->c_str(), std::ios::in|std::ios::binary);
    msgbuffer.seekg(pos);
    msgbuffer.read((char*)buf, size);
    msgbuffer.close();
}

void ParseMsg::ReturnUint32(uint32_t* tmp32, uint8_t* tmp8, bool isbigendian)
{
    if(isbigendian)
        *tmp32 = __builtin_bswap32((uint32_t)tmp8[0] | (uint32_t)tmp8[1] << 8 | (uint32_t)tmp8[2] << 16 | (uint32_t)tmp8[3] << 24);
    else
        *tmp32 = (uint32_t)tmp8[0] | (uint32_t)tmp8[1] << 8 | (uint32_t)tmp8[2] << 16 | (uint32_t)tmp8[3] << 24;
}

void ParseMsg::ReturnUint16(uint16_t* tmp16, uint8_t* tmp8, bool isbigendian)
{
    if(isbigendian)
        *tmp16 = __builtin_bswap16((uint16_t)tmp8[0] | (uint16_t)tmp8[1] << 8);
    else
        *tmp16 = (uint16_t)tmp8[0] | (uint16_t)tmp8[1] << 8;
}

void ParseMsg::ReturnUint64(uint64_t* tmp64, uint8_t* tmp8, bool isbigendian)
{
    if(isbigendian)
        *tmp64 = __builtin_bswap64((uint64_t)tmp8[0] | (uint64_t)tmp8[1] << 8 | (uint64_t)tmp8[2] << 16 | (uint64_t)tmp8[3] << 24 | (uint64_t)tmp8[4] << 32 | (uint64_t)tmp8[5] << 40 | (uint64_t)tmp8[6] << 48 | (uint64_t)tmp8[7] << 56);
    else
        *tmp64 = (uint64_t)tmp8[0] | (uint64_t)tmp8[1] << 8 | (uint64_t)tmp8[2] << 16 | (uint64_t)tmp8[3] << 24 | (uint64_t)tmp8[4] << 32 | (uint64_t)tmp8[5] << 40 | (uint64_t)tmp8[6] << 48 | (uint64_t)tmp8[7] << 56;
}
