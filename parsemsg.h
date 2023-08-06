#ifndef PARSEMSG_H
#define PARSEMSG_H

// Copyright 2023-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstdint>

struct DirectoryEntry
{
    std::string name;
    uint16_t namelength;
    uint8_t objecttype;
    uint8_t colorflag;
    uint32_t leftsiblingid;
    uint32_t rightsiblingid;
    uint32_t childid;
    uint32_t startingsector;
    uint64_t streamsize;
};

class ParseMsg 
{
    private:
        std::string* msgfilepath;
        std::ifstream msgbuffer;
        uint16_t majorversion;
        uint16_t byteorder;
        uint16_t sectorshift;
        uint16_t minisectorshift;
        uint16_t sectorsize;
        uint16_t minisectorsize;
        uint32_t directorysectorcount;
        uint32_t fatsectorcount;
        uint32_t startingdirectorysector;
        uint32_t ministreammaxsize;
        uint32_t startingminifatsector;
        uint32_t minifatsectorcount;
        uint32_t startingdifatsector;
        uint32_t difatsectorcount;
        uint32_t startingministreamsector;
        uint64_t ministreamsize;
        std::vector<uint32_t> fatsectorlocations;
        std::vector<std::vector<uint32_t>> fatchains;
        std::vector<std::vector<uint32_t>> minifatchains;

    protected: 
        ParseMsg() {};
        void GetHeaderValues(void);
        void ReturnUint32(uint32_t* tmp32, uint8_t* tmp8, bool isbigendian=false);
        void ReturnUint16(uint16_t* tmp16, uint8_t* tmp8, bool isbigendian=false);
        void ReturnUint64(uint64_t* tmp64, uint8_t* tmp8, bool isbigendian=false);
        //void ReadContent(uint8_t* buf, uint64_t pos, uint64_t size);
        void ReadContent(uint16_t* val, uint64_t pos, bool isbigendian=false);
        void ReadContent(uint32_t* val, uint64_t pos, bool isbigendian=false);
        void ReadContent(uint64_t* val, uint64_t pos, bool isbigendian=false);
        void ParseFat(void);
        void ParseMiniFat(void);
        void ParseRootDirectory(void);
        void ParseDirectoryEntry(DirectoryEntry* direntry, uint64_t offset);
        void FindDirectoryEntry(std::string direntryname);
        void NavigateDirectoryEntries();

    public:
        ParseMsg(std::string* msgfile);
        ~ParseMsg();
        bool VerifyHeader();
        std::string SenderName(void);
};

#endif // PARSEMSG_H
