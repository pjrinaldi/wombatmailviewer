#include "parsemsg.h"

ParseMsg::ParseMsg(std::string* msgfile)
{
    // PARSE HEADER SECTOR HERE
    msgfilepath = msgfile;
    GetHeaderValues();
    //std::cout << "msgfile: " << *msgfilepath << std::endl;
}

bool ParseMsg::VerifyHeader()
{
    bool ismsg = false;
    uint64_t msghdr = 0;
    ReadContent(&msghdr, 0);
    //uint8_t* tmp8 = new uint8_t[8];
    //ReadContent(tmp8, 0, 8);
    //ReturnUint64(&msghdr, tmp8);
    //delete[] tmp8;
    if(msghdr == 0xe11ab1a1e011cfd0)
        ismsg = true;

    return ismsg;
}

void ParseMsg::GetHeaderValues(void)
{
    majorversion = 0;
    ReadContent(&majorversion, 0x1a);
    if(majorversion == 0x03)
    {
        sectorshift = 0x0009;
        sectorsize = 512;
        directorysectorcount = 0;
    }
    else if(majorversion == 0x04)
    {
        sectorshift = 0x000c;
        sectorsize = 4096;
        directorysectorcount = 0;
        ReadContent(&directorysectorcount, 0x28);
    }
    minisectorshift = 0x0006;
    minisectorsize = 64;
    fatsectorcount = 0;
    ReadContent(&fatsectorcount, 0x2c);
    startingdirectorysector = 0;
    ReadContent(&startingdirectorysector, 0x30);
    ministreammaxsize = 0x1000;
    startingminifatsector = 0;
    ReadContent(&startingminifatsector, 0x3c);
    minifatsectorcount = 0;
    ReadContent(&minifatsectorcount, 0x40);
    startingdifatsector = 0;
    ReadContent(&startingdifatsector, 0x44);
    difatsectorcount = 0;
    ReadContent(&difatsectorcount, 0x48);
    //std::cout << "fat sector count: " << fatsectorcount << std::endl;
    int fatsectorsize = 109;
    if(fatsectorcount < 109)
        fatsectorsize = fatsectorcount;
    for(int i=0; i < fatsectorsize; i++)
    {
        uint32_t curfatsector = 0;
        ReadContent(&curfatsector, 0x4c + i*4);
        if(curfatsector < 0xffffffff)
            fatsectorlocations.push_back(curfatsector);
    }
    if(difatsectorcount > 0)
    {
        std::cout << "process extra difat sectors here" << std::endl;
        //std::cout << "starting difatsector 0x" << startingdifatsector << std::endl;
    }
    ParseFat();
    ParseMiniFat();
    ParseRootDirectory();
}

void ParseMsg::ParseFat(void)
{
    for(int i=0; i < fatsectorlocations.size(); i++)
    {
        std::cout << "item " << i << ": 0x" << std::hex << fatsectorlocations.at(i) << std::dec << std::endl;
        uint16_t fatcnt = sectorsize / 4;
        //uint8_t* curfatbuffer = new uint8_t[sectorsize];
        //ReadContent(curfatbuffer, (fatsectorlocations.at(i) + 1) * sectorsize, sectorsize);
        std::vector<uint32_t> curchain;
        curchain.clear();
        for(uint32_t j=0; j < fatcnt; j++)
        {
            if(j == 0)
                curchain.push_back(0);
            uint32_t nextsector = 0;
            ReadContent(&nextsector, (fatsectorlocations.at(i) + 1) * sectorsize + (j * 4));
            if(nextsector == 0xfffffffe) // ENDOFCHAIN
            {
                fatchains.push_back(curchain);
                curchain.clear();
                curchain.push_back(j+1);
            }
            else if(nextsector == 0xffffffff) // FREESECT
            {
                if(curchain.size() > 0)
                {
                    fatchains.push_back(curchain);
                    curchain.clear();
                }
            }
            else if(nextsector == 0xfffffffc) // DIFSECT
            {
                // should store difat sectors in a difatsectorarray
                if(curchain.size() > 0)
                {
                    fatchains.push_back(curchain);
                    curchain.clear();
                }
            }
            else if(nextsector == 0xfffffffd) // FATSECT
            {
                // should store fat sectors in a fatsectorarray
                if(curchain.size() > 0)
                {
                    fatchains.push_back(curchain);
                    curchain.clear();
                }
            }
            else
                curchain.push_back(nextsector);
        }
    }
    /*
    for(int i=0; i < fatchains.size(); i++)
    {
        std::cout << "chain " << std::dec << i << std::hex << ": ";
        for(int j=0; j < fatchains.at(i).size(); j++)
        {
            std::cout << fatchains.at(i).at(j) << ",";
        }
        std::cout << std::endl;
    }
    */
}

void ParseMsg::ParseMiniFat(void)
{
    std::cout << "starting minifat sector: " << startingminifatsector << std::endl;
    std::cout << "minifat sector count: " << minifatsectorcount << std::endl;
}

void ParseMsg::ParseRootDirectory(void)
{
    std::cout << "starting directory sector: " << startingdirectorysector << std::endl;
}

void ParseMsg::ReadContent(uint8_t* buf, uint64_t pos, uint64_t size)
{
    msgbuffer.open(msgfilepath->c_str(), std::ios::in|std::ios::binary);
    msgbuffer.seekg(pos);
    msgbuffer.read((char*)buf, size);
    msgbuffer.close();
}

void ParseMsg::ReadContent(uint16_t* val, uint64_t pos, bool isbigendian)
{
    uint8_t* tmp8 = new uint8_t[2];
    msgbuffer.open(msgfilepath->c_str(), std::ios::in|std::ios::binary);
    msgbuffer.seekg(pos);
    msgbuffer.read((char*)tmp8, 2);
    msgbuffer.close();
    ReturnUint16(val, tmp8, isbigendian);
    delete[] tmp8;
}

void ParseMsg::ReadContent(uint32_t* val, uint64_t pos, bool isbigendian)
{
    uint8_t* tmp8 = new uint8_t[4];
    msgbuffer.open(msgfilepath->c_str(), std::ios::in|std::ios::binary);
    msgbuffer.seekg(pos);
    msgbuffer.read((char*)tmp8, 4);
    msgbuffer.close();
    ReturnUint32(val, tmp8, isbigendian);
    delete[] tmp8;
}

void ParseMsg::ReadContent(uint64_t* val, uint64_t pos, bool isbigendian)
{
    uint8_t* tmp8 = new uint8_t[8];
    msgbuffer.open(msgfilepath->c_str(), std::ios::in|std::ios::binary);
    msgbuffer.seekg(pos);
    msgbuffer.read((char*)tmp8, 8);
    msgbuffer.close();
    ReturnUint64(val, tmp8, isbigendian);
    delete[] tmp8;
}

/*
void ParseMsg::ReadContent(uint8_t* buf, uint64_t pos, uint64_t size)
{
    msgbuffer.open(msgfilepath->c_str(), std::ios::in|std::ios::binary);
    msgbuffer.seekg(pos);
    msgbuffer.read((char*)buf, size);
    msgbuffer.close();
}
*/

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
