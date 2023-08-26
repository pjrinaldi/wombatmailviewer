#include "cfb.h"

CompoundFileBinary::CompoundFileBinary(std::string* msgfile)
{
    // PARSE HEADER SECTOR HERE
    msgfilepath = msgfile;
    GetHeaderValues();
    //std::cout << "msgfile: " << *msgfilepath << std::endl;
}

bool CompoundFileBinary::VerifyHeader()
{
    bool ismsg = false;
    uint64_t msghdr = 0;
    ReadContent(&msghdr, 0);
    if(msghdr == 0xe11ab1a1e011cfd0)
        ismsg = true;

    return ismsg;
}

void CompoundFileBinary::GetHeaderValues(void)
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

void CompoundFileBinary::ParseFat(void)
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

void CompoundFileBinary::ParseMiniFat(void)
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

void CompoundFileBinary::ParseRootDirectory(void)
{
    //std::cout << "starting directory sector: 0x" << startingdirectorysector << std::endl;
    startingministreamsector = 0;
    ReadContent(&startingministreamsector, (startingdirectorysector + 1) * sectorsize + 0x74);
    ministreamsize = 0;
    ReadContent(&ministreamsize, (startingdirectorysector + 1) * sectorsize + 0x78);
    //std::cout << std::hex << "starting mini stream sector: 0x" << startingministreamsector << std::dec << std::endl;
    //std::cout << "mini stream size: " << ministreamsize << std::endl;
}

void CompoundFileBinary::NavigateDirectoryTree(DirectoryEntry* currententry, std::string direntryname, uint32_t curid)
{
    // MIGHT NEED TO KEEP UP WITH PARID, SO IF THERE IS NO CHILD LEFT OR RIGHT, THEN WE CAN GO BACK TO THE LAST PARENT AND GO NEXT SIBLING INSTEAD OF CHILD.... REQUIRE MORE THOUGHT
    DirectoryEntry curentry;
    if(curid == 0)
        curentry = directoryentries.at(0);
    else
    {
        for(int i=0; i < directoryentries.size(); i++)
        {
            //std::cout << "curid: " << curid << " id: " << directoryentries.at(i).id << " name: " << directoryentries.at(i).name << " to mach: " << direntryname << std::endl;
            if(curid == directoryentries.at(i).id)
            {
                curentry = directoryentries.at(i);
                break;
            }
        }
    }
    if(curentry.name.find(direntryname) != std::string::npos)
    {
        //std::cout << "match" << std::endl;
        *currententry = curentry;
    }
    else if(curid == 0xffffffff)
    {
       //std::cout << "stream not found" << std::endl;
    }
    else
    {
        //std::cout << "no match." << std::endl;
        //std::cout << "old id: " << curid << std::endl;
        if(curid == 0)// || (curentry.rightsiblingid == 0xffffffff && curentry.childid != 0xffffffff))
            curid = curentry.childid;
        else
            curid = curentry.rightsiblingid;
        //std::cout << "new id: " << curid << std::endl;
        NavigateDirectoryTree(currententry, direntryname, curid);
    }
}

/*
void CompoundFileBinary::GetDirectoryEntryStream(DirectoryEntry* curdirentry, std::string* direntrystream)
{
    *direntrystream = "";
    // GET CURRENT DIRECTORY ENTRY
    // MOVE NAVIGATEDIRECTORYTREE HERE...
    //for(int i=0; i < directoryentries.size(); i++)
    //{
    //    if(directoryentries.at(i).name.find(direntryname) != std::string::npos)
    //    {
    //        curdirentry = directoryentries.at(i);
    //        break;
    //    }
    //}
    if(curdirentry->streamsize == 0) // DO NOTHING
    {
    }
    else if(curdirentry->streamsize < 4096) // USE MINI STREAMS
    {
        // GET MINI FAT CHAIN FROM FATCHAIN
        std::vector<uint32_t> minifatchain;
        minifatchain.clear();
        for(int i=0; i < fatchains.size(); i++)
        {
            if(fatchains.at(i).at(0) == startingministreamsector)
            {
                minifatchain = fatchains.at(i);
                break;
            }
        }

        //std::cout << "mini fat chain: ";
        //for(int i=0; i < minifatchain.size(); i++)
        //    std::cout << minifatchain.at(i) << ", ";
        //std::cout << std::endl;

        if(curdirentry->streamsize < 64) // don't need minifatchains, value is contained in 1 minifat sector
        {
            //std::cout << "curdirentry starting sector: " << curdirentry.startingsector << std::endl;
            uint16_t sectorcnt = curdirentry->startingsector / 8;
            if(curdirentry->startingsector % 8 != 0)
                sectorcnt++;
            //std::cout << curdirentry.startingsector / 8 << " " << curdirentry.startingsector % 8 << std::endl;
            uint64_t curoffset = 0;
            if(curdirentry->startingsector % 8 == 0)
                curoffset = ((minifatchain.at(sectorcnt - 1)) + 2) * sectorsize;
            else
                curoffset = ((minifatchain.at(sectorcnt - 1)) + 1) * sectorsize + (curdirentry->startingsector % 8) * 64;
            //std::cout << "minifatchain offset: " << (minifatchain.at(sectorcnt - 1) + 1) * sectorsize << std::endl;
            //std::cout << "curoffset: " << curoffset << std::endl;
            if(curdirentry->name.find("001E") != std::string::npos) // UTF-8
            {
                uint8_t* tmpbuf = new uint8_t[curdirentry->streamsize+1];
                ReadContent(tmpbuf, curoffset, curdirentry->streamsize);
                tmpbuf[curdirentry->streamsize] = '\0';
                *direntrystream += (char*)tmpbuf;
                delete[] tmpbuf;
                //std::cout << "utf-8: |" << *direntrystream << "|" << std::endl;
            }
            else if(curdirentry->name.find("001F") != std::string::npos) // UTF-16
            {
                for(int i=0; i < curdirentry->streamsize / 2; i++)
                {
                    uint16_t singleletter = 0;
                    ReadContent(&singleletter, curoffset + i*2);
                    *direntrystream += (char)singleletter;
                }
                //std::cout << "utf-16: |" << *direntrystream << "|" << std::endl;
            }
        }
        else // ensure the minifatchains are sequential to get the values
        {
        }
    }
    else // USE REGULAR SECTORS
    {
        // get chain from the fat based off of the starting sector (for/if for 1st entry of each fatchains)
        // then use the stream size to read the content into the respective storage container.
        std::cout << "use regular sectors for stream: " << curdirentry->startingsector << std::endl;
    }
}
*/

void CompoundFileBinary::GetDirectoryEntry(DirectoryEntry* currententry, uint32_t curid)
{
    for(int i=0; i < directoryentries.size(); i++)
    {
        if(directoryentries.at(i).id == curid)
        {
            *currententry = directoryentries.at(i);
            break;
        }
    }
}

void CompoundFileBinary::GetEntryStream(DirectoryEntry* curdirentry, std::string* entrystream)
{
    if(curdirentry->streamsize > 0 && curdirentry->streamsize < 4096) // USE MINI STREAMS
    {
        // GET MINI FAT CHAIN FROM FATCHAIN
        std::vector<uint32_t> minifatchain;
        minifatchain.clear();
        for(int i=0; i < fatchains.size(); i++)
        {
            if(fatchains.at(i).at(0) == startingministreamsector)
            {
                minifatchain = fatchains.at(i);
                break;
            }
        }

        /*
        std::cout << "mini fat chain: ";
        for(int i=0; i < minifatchain.size(); i++)
            std::cout << minifatchain.at(i) << ", ";
        std::cout << std::endl;
        */

        // ASSUME THE MINIFAT CHAINS FOR STREAMS ARE CONTINUOUS
        //if(curdirentry.streamsize < 4096) // don't need minifatchains, value is contained in 1 minifat sector
        //{
        //std::cout << "curdirentry starting sector: " << curdirentry.startingsector << std::endl;
        uint16_t sectorcnt = curdirentry->startingsector / 8;
        if(curdirentry->startingsector % 8 != 0)
            sectorcnt++;
        //std::cout << curdirentry.startingsector / 8 << " " << curdirentry.startingsector % 8 << std::endl;
        uint64_t curoffset = 0;
        if(curdirentry->startingsector % 8 == 0)
            curoffset = ((minifatchain.at(sectorcnt - 1)) + 2) * sectorsize;
        else
            curoffset = ((minifatchain.at(sectorcnt - 1)) + 1) * sectorsize + (curdirentry->startingsector % 8) * 64;
        //std::cout << "minifatchain offset: " << (minifatchain.at(sectorcnt - 1) + 1) * sectorsize << std::endl;
        //std::cout << "curoffset: " << curoffset << std::endl;
        if(curdirentry->name.find("001E") != std::string::npos) // UTF-8
        {
            uint8_t* tmpbuf = new uint8_t[curdirentry->streamsize+1];
            ReadContent(tmpbuf, curoffset, curdirentry->streamsize);
            tmpbuf[curdirentry->streamsize] = '\0';
            *entrystream += (char*)tmpbuf;
            delete[] tmpbuf;
            //std::cout << "utf-8: |" << *direntrystream << "|" << std::endl;
        }
        else if(curdirentry->name.find("001F") != std::string::npos) // UTF-16
        {
            for(int i=0; i < curdirentry->streamsize / 2; i++)
            {
                uint16_t singleletter = 0;
                ReadContent(&singleletter, curoffset + i*2);
                *entrystream += (char)singleletter;
            }
            //std::cout << "utf-16: |" << *direntrystream << "|" << std::endl;
        }
        /*
        }
        else // ensure the minifatchains are sequential to get the values
        {
        }
        */
    }
    /*
    else // USE SECTOR STREAMS
    {
        for(int i=0; i < fatchains.size(); i++)
        {
            if(fatchains.at(i).at(0) == curdirentry->startingsector)
            {
            }

        }
    }
    */
}

//void CompoundFileBinary::FindDirectoryEntry(std::string direntryname)
void CompoundFileBinary::GetDirectoryEntryStream(std::string* direntrystream, std::string direntryname)
{
    DirectoryEntry curdirentry;
    *direntrystream = "";
    // GET CURRENT DIRECTORY ENTRY
    // MOVE NAVIGATEDIRECTORYTREE HERE INSTEAD OF THIS GENERIC FOR LOOP
    /*
    for(int i=0; i < directoryentries.size(); i++)
    {
        if(directoryentries.at(i).name.find(direntryname) != std::string::npos)
        {
            curdirentry = directoryentries.at(i);
            break;
        }
    }
    */
    NavigateDirectoryTree(&curdirentry, direntryname, 0);
    if(curdirentry.streamsize == 0) // DO NOTHING
    {
    }
    else if(curdirentry.streamsize < 4096) // USE MINI STREAMS
    {
        // GET MINI FAT CHAIN FROM FATCHAIN
        std::vector<uint32_t> minifatchain;
        minifatchain.clear();
        for(int i=0; i < fatchains.size(); i++)
        {
            if(fatchains.at(i).at(0) == startingministreamsector)
            {
                minifatchain = fatchains.at(i);
                break;
            }
        }

        /*
        std::cout << "mini fat chain: ";
        for(int i=0; i < minifatchain.size(); i++)
            std::cout << minifatchain.at(i) << ", ";
        std::cout << std::endl;
        */

        // ASSUME THE MINIFAT CHAINS FOR STREAMS ARE CONTINUOUS
        if(curdirentry.streamsize < 4096) // don't need minifatchains, value is contained in 1 minifat sector
        {
            //std::cout << "curdirentry starting sector: " << curdirentry.startingsector << std::endl;
            uint16_t sectorcnt = curdirentry.startingsector / 8;
            if(curdirentry.startingsector % 8 != 0)
                sectorcnt++;
            //std::cout << curdirentry.startingsector / 8 << " " << curdirentry.startingsector % 8 << std::endl;
            uint64_t curoffset = 0;
            if(curdirentry.startingsector % 8 == 0)
                curoffset = ((minifatchain.at(sectorcnt - 1)) + 2) * sectorsize;
            else
                curoffset = ((minifatchain.at(sectorcnt - 1)) + 1) * sectorsize + (curdirentry.startingsector % 8) * 64;
            //std::cout << "minifatchain offset: " << (minifatchain.at(sectorcnt - 1) + 1) * sectorsize << std::endl;
            //std::cout << "curoffset: " << curoffset << std::endl;
            if(curdirentry.name.find("001E") != std::string::npos) // UTF-8
            {
                uint8_t* tmpbuf = new uint8_t[curdirentry.streamsize+1];
                ReadContent(tmpbuf, curoffset, curdirentry.streamsize);
                tmpbuf[curdirentry.streamsize] = '\0';
                *direntrystream += (char*)tmpbuf;
                delete[] tmpbuf;
                //std::cout << "utf-8: |" << *direntrystream << "|" << std::endl;
            }
            else if(curdirentry.name.find("001F") != std::string::npos) // UTF-16
            {
                for(int i=0; i < curdirentry.streamsize / 2; i++)
                {
                    uint16_t singleletter = 0;
                    ReadContent(&singleletter, curoffset + i*2);
                    *direntrystream += (char)singleletter;
                }
                //std::cout << "utf-16: |" << *direntrystream << "|" << std::endl;
            }
        }
        else // ensure the minifatchains are sequential to get the values
        {
        }
    }
    else // USE REGULAR SECTORS
    {
        // get chain from the fat based off of the starting sector (for/if for 1st entry of each fatchains)
        // then use the stream size to read the content into the respective storage container.
        //std::cout << "use regular sectors for stream: " << curdirentry.startingsector << std::endl;
    }
}

void CompoundFileBinary::GetEntryBuffer(DirectoryEntry* curdirentry, uint8_t** entrybuffer)
{
    if(curdirentry->streamsize > 0 && curdirentry->streamsize < 4096)
    {
        // GET MINI FAT CHAIN FROM FATCHAIN
        std::vector<uint32_t> minifatchain;
        minifatchain.clear();
        for(int i=0; i < fatchains.size(); i++)
        {
            if(fatchains.at(i).at(0) == startingministreamsector)
            {
                minifatchain = fatchains.at(i);
                break;
            }
        }

        /*
        std::cout << "mini fat chain: ";
        for(int i=0; i < minifatchain.size(); i++)
            std::cout << minifatchain.at(i) << ", ";
        std::cout << std::endl;
        */

        uint16_t sectorcnt = curdirentry->startingsector / 8;
        if(curdirentry->startingsector % 8 != 0)
            sectorcnt++;
        //std::cout << curdirentry.startingsector / 8 << " " << curdirentry.startingsector % 8 << std::endl;
        uint64_t curoffset = 0;
        if(curdirentry->startingsector % 8 == 0)
            curoffset = ((minifatchain.at(sectorcnt - 1)) + 2) * sectorsize;
        else
            curoffset = ((minifatchain.at(sectorcnt - 1)) + 1) * sectorsize + (curdirentry->startingsector % 8) * 64;

        //std::cout << "curoffset " << curoffset << std::endl;
        //uint8_t* tmpbuf = new uint8_t[curdirentry.streamsize];
        //*pbsize = curdirentry->streamsize;
        *entrybuffer = new uint8_t[curdirentry->streamsize];
        ReadContent(*entrybuffer, curoffset, curdirentry->streamsize);
        //ReadContent(tmpbuf, curoffset, curdirentry.streamsize);
        //std::cout << "buf: " << (uint)tmpbuf[8] << std::endl;
        //std::cout << "buf: " << (uint)buffer[8] << std::endl;
    }
    else if(curdirentry->streamsize >= 4096)
    {
        for(int i=0; i < fatchains.size(); i++)
        {
            if(fatchains.at(i).at(0) == curdirentry->startingsector)
            {
                *entrybuffer = new uint8_t[curdirentry->streamsize];
                ReadContent(*entrybuffer, (curdirentry->startingsector + 1) * sectorsize, curdirentry->streamsize);
            }
        }
    }
}

void CompoundFileBinary::GetDirectoryEntryBuffer(uint8_t** buffer, uint64_t* pbsize, std::string direntryname)
{
    DirectoryEntry curdirentry;
    // GET CURRENT DIRECTORY ENTRY
    NavigateDirectoryTree(&curdirentry, direntryname, 0);
    //std::cout << "stream offset: " << curdirentry.startingsector << " stream size: " << curdirentry.streamsize << std::endl;
    if(curdirentry.streamsize == 0) // DO NOTHING
    {
    }
    else if(curdirentry.streamsize < 4096) // USE MINI STREAMS
    {
        // GET MINI FAT CHAIN FROM FATCHAIN
        std::vector<uint32_t> minifatchain;
        minifatchain.clear();
        for(int i=0; i < fatchains.size(); i++)
        {
            if(fatchains.at(i).at(0) == startingministreamsector)
            {
                minifatchain = fatchains.at(i);
                break;
            }
        }

        /*
        std::cout << "mini fat chain: ";
        for(int i=0; i < minifatchain.size(); i++)
            std::cout << minifatchain.at(i) << ", ";
        std::cout << std::endl;
        */

        uint16_t sectorcnt = curdirentry.startingsector / 8;
        if(curdirentry.startingsector % 8 != 0)
            sectorcnt++;
        //std::cout << curdirentry.startingsector / 8 << " " << curdirentry.startingsector % 8 << std::endl;
        uint64_t curoffset = 0;
        if(curdirentry.startingsector % 8 == 0)
            curoffset = ((minifatchain.at(sectorcnt - 1)) + 2) * sectorsize;
        else
            curoffset = ((minifatchain.at(sectorcnt - 1)) + 1) * sectorsize + (curdirentry.startingsector % 8) * 64;

        //std::cout << "curoffset " << curoffset << std::endl;
        //uint8_t* tmpbuf = new uint8_t[curdirentry.streamsize];
        *pbsize = curdirentry.streamsize;
        *buffer = new uint8_t[curdirentry.streamsize];
        ReadContent(*buffer, curoffset, curdirentry.streamsize);
        //ReadContent(tmpbuf, curoffset, curdirentry.streamsize);
        //std::cout << "buf: " << (uint)tmpbuf[8] << std::endl;
        //std::cout << "buf: " << (uint)buffer[8] << std::endl;
    }
    else // USE FAT SECTORS
    {
    }
}

void CompoundFileBinary::NavigateDirectoryEntries(void)
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
    /*
    for(int i=0; i < directoryentries.size(); i++)
    {
        std::cout << std::hex << "id: " << directoryentries.at(i).id << " name: " << directoryentries.at(i).name << " rid: " << directoryentries.at(i).rightsiblingid << " cid: " << directoryentries.at(i).childid << std::dec << std::endl;
    }
    std::cout << "directoryentries count: " << directoryentries.size() << std::endl;
    */
}

void CompoundFileBinary::ParseDirectoryEntry(DirectoryEntry* direntry, uint64_t offset)
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

void CompoundFileBinary::ReadContent(uint8_t* buf, uint64_t pos, uint64_t size)
{
    msgbuffer.open(msgfilepath->c_str(), std::ios::in|std::ios::binary);
    msgbuffer.seekg(pos);
    msgbuffer.read((char*)buf, size);
    msgbuffer.close();
}

void CompoundFileBinary::ReadContent(uint16_t* val, uint64_t pos, bool isbigendian)
{
    uint8_t* tmp8 = new uint8_t[2];
    msgbuffer.open(msgfilepath->c_str(), std::ios::in|std::ios::binary);
    msgbuffer.seekg(pos);
    msgbuffer.read((char*)tmp8, 2);
    msgbuffer.close();
    ReturnUint16(val, tmp8, isbigendian);
    delete[] tmp8;
}

void CompoundFileBinary::ReadContent(uint32_t* val, uint64_t pos, bool isbigendian)
{
    uint8_t* tmp8 = new uint8_t[4];
    msgbuffer.open(msgfilepath->c_str(), std::ios::in|std::ios::binary);
    msgbuffer.seekg(pos);
    msgbuffer.read((char*)tmp8, 4);
    msgbuffer.close();
    ReturnUint32(val, tmp8, isbigendian);
    delete[] tmp8;
}

void CompoundFileBinary::ReadContent(uint64_t* val, uint64_t pos, bool isbigendian)
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
void CompoundFileBinary::ReadContent(uint8_t* buf, uint64_t pos, uint64_t size)
{
    msgbuffer.open(msgfilepath->c_str(), std::ios::in|std::ios::binary);
    msgbuffer.seekg(pos);
    msgbuffer.read((char*)buf, size);
    msgbuffer.close();
}
*/

void CompoundFileBinary::ReturnUint32(uint32_t* tmp32, uint8_t* tmp8, bool isbigendian)
{
    if(isbigendian)
        *tmp32 = __builtin_bswap32((uint32_t)tmp8[0] | (uint32_t)tmp8[1] << 8 | (uint32_t)tmp8[2] << 16 | (uint32_t)tmp8[3] << 24);
    else
        *tmp32 = (uint32_t)tmp8[0] | (uint32_t)tmp8[1] << 8 | (uint32_t)tmp8[2] << 16 | (uint32_t)tmp8[3] << 24;
}

void CompoundFileBinary::ReturnUint16(uint16_t* tmp16, uint8_t* tmp8, bool isbigendian)
{
    if(isbigendian)
        *tmp16 = __builtin_bswap16((uint16_t)tmp8[0] | (uint16_t)tmp8[1] << 8);
    else
        *tmp16 = (uint16_t)tmp8[0] | (uint16_t)tmp8[1] << 8;
}

void CompoundFileBinary::ReturnUint64(uint64_t* tmp64, uint8_t* tmp8, bool isbigendian)
{
    if(isbigendian)
        *tmp64 = __builtin_bswap64((uint64_t)tmp8[0] | (uint64_t)tmp8[1] << 8 | (uint64_t)tmp8[2] << 16 | (uint64_t)tmp8[3] << 24 | (uint64_t)tmp8[4] << 32 | (uint64_t)tmp8[5] << 40 | (uint64_t)tmp8[6] << 48 | (uint64_t)tmp8[7] << 56);
    else
        *tmp64 = (uint64_t)tmp8[0] | (uint64_t)tmp8[1] << 8 | (uint64_t)tmp8[2] << 16 | (uint64_t)tmp8[3] << 24 | (uint64_t)tmp8[4] << 32 | (uint64_t)tmp8[5] << 40 | (uint64_t)tmp8[6] << 48 | (uint64_t)tmp8[7] << 56;
}
