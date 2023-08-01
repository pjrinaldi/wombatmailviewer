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

    protected: 
        ParseMsg() {};

    public:
        ParseMsg(std::string* msgfile);
        ~ParseMsg();
        bool VerifyHeader();
        void ReturnUint32(uint32_t* tmp32, uint8_t* tmp8, bool isbigendian=false);
        void ReturnUint16(uint16_t* tmp16, uint8_t* tmp8, bool isbigendian=false);
        void ReturnUint64(uint64_t* tmp64, uint8_t* tmp8, bool isbigendian=false);
        void ReadContent(uint8_t* buf, uint64_t pos, uint64_t size);
};

#endif // PARSEMSG_H
