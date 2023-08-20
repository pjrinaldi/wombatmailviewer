#ifndef COMPOUNDFILEBINARY_H
#define COMPOUNDFILEBINARY_H

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
    uint32_t id;
    std::string name;
    uint32_t leftsiblingid;
    uint32_t rightsiblingid;
    uint32_t childid;
    uint32_t startingsector;
    uint64_t streamsize;
};

class CompoundFileBinary 
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
        std::vector<DirectoryEntry> directoryentries;

    protected: 
        CompoundFileBinary() {};
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
        void ParseDirectoryEntry(DirectoryEntry* direntry, uint64_t offset);

    public:
        CompoundFileBinary(std::string* msgfile);
        ~CompoundFileBinary();
        bool VerifyHeader();
        void NavigateDirectoryEntries();
        void GetDirectoryEntry(DirectoryEntry* currententry, uint32_t curid);
        void GetEntryStream(DirectoryEntry* currententry, std::string* entrystream);
        void GetEntryBuffer(DirectoryEntry* currententry, uint8_t** entrybuffer);
        void GetDirectoryEntryStream(std::string* direntrystream, std::string direntryname);
        //void GetDirectoryEntryStream(DirectoryEntry* currententry, std::string* direntrystream);
        //void FindDirectoryEntry(std::string direntryname);
        void NavigateDirectoryTree(DirectoryEntry* currententry, std::string direntyname, uint32_t curid);
        void GetDirectoryEntryBuffer(uint8_t** buffer, uint64_t* pbsize, std::string direntryname);
};

#endif // COMPUNDFILEBINARY_H
