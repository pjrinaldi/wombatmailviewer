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
        std::vector<uint32_t> fatsectorlocations;
        std::vector<std::vector<uint32_t>> fatchains;

    protected: 
        ParseMsg() {};
        void GetHeaderValues(void);
        void ReturnUint32(uint32_t* tmp32, uint8_t* tmp8, bool isbigendian=false);
        void ReturnUint16(uint16_t* tmp16, uint8_t* tmp8, bool isbigendian=false);
        void ReturnUint64(uint64_t* tmp64, uint8_t* tmp8, bool isbigendian=false);
        void ReadContent(uint8_t* buf, uint64_t pos, uint64_t size);
        void ReadContent(uint16_t* val, uint64_t pos, bool isbigendian=false);
        void ReadContent(uint32_t* val, uint64_t pos, bool isbigendian=false);
        void ReadContent(uint64_t* val, uint64_t pos, bool isbigendian=false);
        void ParseFat(void);
        void ParseMiniFat(void);
        void ParseRootDirectory(void);

    public:
        ParseMsg(std::string* msgfile);
        ~ParseMsg();
        bool VerifyHeader();
};

#endif // PARSEMSG_H
