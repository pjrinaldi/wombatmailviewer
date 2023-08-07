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
        //std::cout << "item " << i << ": 0x" << std::hex << fatsectorlocations.at(i) << std::dec << std::endl;
        uint16_t fatcnt = sectorsize / 4;
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
    //std::cout << "starting minifat sector: " << startingminifatsector << std::endl;
    //std::cout << "minifat sector count: " << minifatsectorcount << std::endl;
    // FIND MINIFAT FAT CHAIN
    int fatchainforminifat = 0;
    for(int i=0; i < fatchains.size(); i++)
    {
        if(startingminifatsector == fatchains.at(i).at(0))
        {
            fatchainforminifat = i;
            //std::cout << "fat chain for minifat is at i: " << i << std::endl;
            break;
        }
    }
    // PARSE MINIFAT CHAINS
    for(int i=0; i < fatchains.at(fatchainforminifat).size(); i++)
    {
        uint16_t minifatcnt = sectorsize / 4;
        std::vector<uint32_t> curchain;
        curchain.clear();
        for(uint32_t j=0; j < minifatcnt; j++)
        {
            if(j == 0)
                curchain.push_back(0);
            uint32_t nextsector = 0;
            ReadContent(&nextsector, (fatchains.at(fatchainforminifat).at(i) + 1) * sectorsize + (j * 4));
            if(nextsector == 0xfffffffe) // ENDOFCHAIN
            {
                minifatchains.push_back(curchain);
                curchain.clear();
                curchain.push_back(j+1);
            }
            else if(nextsector == 0x00000000)
                curchain.clear();
            else
                curchain.push_back(nextsector);
        }
    }
    /*
    for(int i=0; i < minifatchains.size(); i++)
    {
        std::cout << "mini fat chain " << std::dec << i << std::hex << ": ";
        for(int j=0; j < minifatchains.at(i).size(); j++)
        {
            std::cout << minifatchains.at(i).at(j) << ",";
        }
        std::cout << std::endl;
    }
    */
}

void ParseMsg::ParseRootDirectory(void)
{
    //std::cout << "starting directory sector: 0x" << startingdirectorysector << std::endl;
    startingministreamsector = 0;
    ReadContent(&startingministreamsector, (startingdirectorysector + 1) * sectorsize + 0x74);
    ministreamsize = 0;
    ReadContent(&ministreamsize, (startingdirectorysector + 1) * sectorsize + 0x78);
    //std::cout << std::hex << "starting mini stream sector: 0x" << startingministreamsector << std::dec << std::endl;
    //std::cout << "mini stream size: " << ministreamsize << std::endl;
}

//void ParseMsg::FindDirectoryEntry(std::string direntryname)
void ParseMsg::GetDirectoryEntryStream(std::string* direntrystream, std::string direntryname)
{
    *direntrystream = "";
    for(int i=0; i < directoryentries.size(); i++)
    {
        if(directoryentries.at(i).name.find(direntryname) != std::string::npos)
        {
            //std::cout << "match for " << directoryentries.at(i).name << " contains " << direntryname << std::endl
            if(directoryentries.at(i).streamsize < 4096) // USE MINI STREAM
            {
                //std::cout << "id: " << directoryentries.at(i).id << " stream size: " << directoryentries.at(i).streamsize << std::endl;
                // get chain from mini fat based off of starting sector
                // then use the stream size to read the content
                // also need to check if it is 001E (UTF-8) or 001F (UTF-16) so i can parse the stream accordingly.
                std::cout << "use mini sectors for stream: " << directoryentries.at(i).startingsector << std::endl;
                for(int j=0; j < minifatchains.size(); j++)
                {
                    if(minifatchains.at(j).at(0) == directoryentries.at(i).startingsector)
                    {
                        std::cout << minifatchains.at(j).size() << " " << directoryentries.at(i).streamsize << std::endl;
                        if(directoryentries.at(i).name.find("001E") != std::string::npos) // UTF-8
                        {
                        }
                        else if(directoryentries.at(i).name.find("001F") != std::string::npos) // UTF-16
                        {
                        }
                        break;
                    }
                }
            }
            else // USE REGULAR SECTORS
            {
                // get chain from the fat based off of the starting sector (for/if for 1st entry of each fatchains)
                // then use the stream size to read the content into the respective storage container.
                std::cout << "use regular sectors for stream: " << directoryentries.at(i).startingsector << std::endl;
            }
        }
    }
}

void ParseMsg::NavigateDirectoryEntries(void)
{
    // FIND THE DIRECTORY ENTRIES FAT CHAIN TO LOOP OVER THEM
    int fatchainfordirectoryentries = 0;
    for(int i=0; i < fatchains.size(); i++)
    {
        if(startingdirectorysector == fatchains.at(i).at(0))
        {
            fatchainfordirectoryentries = i;
            //std::cout << "fat chain for directory entries is at i: " << i << std::endl;
            break;
        }
    }
    int direntrycnt = sectorsize / 128;
    // START THE PROCESS TO NAVIGATE THE DIRECTORY ENTRIES
    int a = 0;
    for(int i=0; i < fatchains.at(fatchainfordirectoryentries).size(); i++)
    {
        uint32_t initialsector = fatchains.at(fatchainfordirectoryentries).at(i);
        //std::cout << std::dec << "initial sector: " << initialsector << std::endl;
        for(int j=0; j < direntrycnt; j++)
        {
            DirectoryEntry curdirentry;
            curdirentry.id = a;
            ParseDirectoryEntry(&curdirentry, ((initialsector + 1) * sectorsize) + (j * 128));
            //std::cout << curdirentry.id << " " << curdirentry.name << " lid: " << curdirentry.leftsiblingid << " rid: " << curdirentry.rightsiblingid << " cid: " << curdirentry.childid << std::endl;
            //std::cout << curdirentry.id << " starting sector: " << curdirentry.startingsector << " stream size: " << curdirentry.streamsize << std::endl;
            directoryentries.push_back(curdirentry);
            a++;
        }
    }
    //std::cout << "directoryentries count: " << directoryentries.size() << std::endl;
}

void ParseMsg::ParseDirectoryEntry(DirectoryEntry* direntry, uint64_t offset)
{
    // directory entry name length is at offset 40, 2 bytes
    uint16_t namelength = 0;
    ReadContent(&namelength, offset + 0x40);
    direntry->name = "";
    for(int i=0; i < namelength / 2; i++)
    {
        uint16_t singleletter = 0;
        ReadContent(&singleletter, offset + i*2);
        direntry->name += (char)singleletter;
    }
    direntry->leftsiblingid = 0;
    ReadContent(&(direntry->leftsiblingid), offset + 0x44);
    direntry->rightsiblingid = 0;
    ReadContent(&(direntry->rightsiblingid), offset + 0x48);
    direntry->childid = 0;
    ReadContent(&(direntry->childid), offset + 0x4c);
    direntry->startingsector = 0;
    ReadContent(&(direntry->startingsector), offset + 0x74);
    direntry->streamsize = 0;
    ReadContent(&(direntry->streamsize), offset + 0x78);
    //std::cout << "direntry stream size: " << direntry->streamsize << std::endl;
    //std::cout << "offset: " << offset << std::endl;
}

std::string ParseMsg::SenderName(void)
{
    NavigateDirectoryEntries();
    std::string sendername = "";
    //FindDirectoryEntry("0C1A");
    GetDirectoryEntryStream(&sendername, "0C1A");
    /*
        m_SenderName = getStringFromStream("__substg1.0_0C1A001F");
        if (m_SenderName.empty())
            m_SenderName = getStringFromStream("__substg1.0_3FFA001F");
        if (m_SenderName.empty())
            m_SenderName = getStringFromStream("__substg1.0_0042001F");
        if (m_SenderName.empty())
            m_SenderName = getString8FromStream("__substg1.0_0C1A001E");
        if (m_SenderName.empty())
            m_SenderName = getString8FromStream("__substg1.0_0042001E");
        if (m_SenderName.empty())
            m_SenderName = getString8FromStream("__substg1.0_3FFA001E");
     */ 

    return sendername;
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
