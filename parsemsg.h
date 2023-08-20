#ifndef PARSEMSG_H
#define PARSEMSG_H

// Copyright 2023-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstdint>

#include "cfb.h"

#define TICKS_PER_SECOND 10000000
#define EPOCH_DIFFERENCE 11644473600LL
#define NSEC_BTWN_1904_1970	(uint32_t) 2082844800U

struct AttachmentInfo
{
    //uint8_t id;             // Attachment Id
    uint32_t dataid;        // 0x3701 // 0x0102 - Binary data type
    //std::string extension;  // 0x3703 // 0x001F/1E - String
    std::string name;       // 0x3704 // 0x001F/1E - String
    std::string longname;   // 0x3707 // 0x001F/1E - String
    std::string mimetag;    // 0x370E // 0x001F/1E - String
    std::string contentid;  // 0x3712 // 0x001F/1E - String
};

std::string SenderName(std::string* mailboxpath);
std::string SenderAddress(std::string* mailboxpath);
void ReceiverNames(std::string* mailboxpath, std::vector<std::string>* receivernames);
void ReceiverAddresses(std::string* mailboxpath, std::vector<std::string>* receiveraddresses);
std::string CarbonCopy(std::string* mailboxpath);
std::string BlindCarbonCopy(std::string* mailboxpath);
std::string Subject(std::string* mailboxpath);
std::string Date(std::string* mailboxpath);
std::string Body(std::string* mailboxpath);
std::string TransportHeader(std::string* mailboxpath);
void AttachmentCount(uint32_t* attachcount, std::string* mailboxpath);
void GetMsgAttachments(std::vector<AttachmentInfo>* msgattachments, uint32_t attachcount, std::string* mailboxpath);

uint8_t* substr(uint8_t* arr, int begin, int len);
void ReturnUint32(uint32_t* tmp32, uint8_t* tmp8, bool isbigendian=false);
void ReturnUint16(uint16_t* tmp16, uint8_t* tmp8, bool isbigendian=false);
void ReturnUint64(uint64_t* tmp64, uint8_t* tmp8, bool isbigendian=false);
void ReadInteger(uint8_t* arr, int begin, uint16_t* val, bool isbigendian=false);
void ReadInteger(uint8_t* arr, int begin, uint32_t* val, bool isbigendian=false);
void ReadInteger(uint8_t* arr, int begin, uint64_t* val, bool isbigendian=false);
std::string ConvertWindowsTimeToUnixTimeUTC(uint64_t input);

#endif // PARSEMSG_H
