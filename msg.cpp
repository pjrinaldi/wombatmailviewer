#include "msg.h"

OutlookMessage::OutlookMessage(std::string* msgfile)
{
    cfb = new CompoundFileBinary(msgfile);
}
/*
std::string SenderName(std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string sendername = "";
    DirectoryEntry curentry;
    // need to move the navigatedirectorytree into getdirectoryentrystream
    //cfb->NavigateDirectoryTree(&curentry, "0C1A", 0);
    //cfb->GetDirectoryEntryStream(&curentry, &sendername);
    //std::cout << "sender name: " << sendername << std::endl;
    cfb->GetDirectoryEntryStream(&sendername, "0C1A");
    if(sendername.empty())
        cfb->GetDirectoryEntryStream(&sendername, "3FFA");
    if(sendername.empty())
        cfb->GetDirectoryEntryStream(&sendername, "0042");

    return sendername;
}

std::string SenderAddress(std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string senderaddress = "";
    cfb->GetDirectoryEntryStream(&senderaddress, "0065");
    if(senderaddress.empty())
        cfb->GetDirectoryEntryStream(&senderaddress, "0C1F");
    if(senderaddress.empty())
        cfb->GetDirectoryEntryStream(&senderaddress, "800B");
    if(senderaddress.empty())
        cfb->GetDirectoryEntryStream(&senderaddress, "3FFA");
    //if(senderaddress.empty())
    //    cfb->GetDirectoryEntryStream(&senderaddress, "5D01");
    if(senderaddress.empty())
        cfb->GetDirectoryEntryStream(&senderaddress, "5D02");
    if(senderaddress.empty())
        cfb->GetDirectoryEntryStream(&senderaddress, "3003");

    return senderaddress;
}

void ReceiverNames(std::string* mailboxpath, std::vector<std::string>* receivernames)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string rnameslist = "";
    cfb->GetDirectoryEntryStream(&rnameslist, "0E04");

    if(!rnameslist.empty())
    {
        std::istringstream rnstream(rnameslist);
        std::string rn;
        while(getline(rnstream, rn, ';'))
            receivernames->push_back(rn);
    }
}

void ReceiverAddresses(std::string* mailboxpath, std::vector<std::string>* receiveraddresses)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string raddrlist = "";
    cfb->GetDirectoryEntryStream(&raddrlist, "5D01");
    if(raddrlist.empty())
        cfb->GetDirectoryEntryStream(&raddrlist, "5D09");

    if(!raddrlist.empty())
    {
        std::istringstream rastream(raddrlist);
        std::string ra;
        while(getline(rastream, ra, ';'))
            receiveraddresses->push_back(ra);
    }
}

std::string CarbonCopy(std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string cc = "";
    cfb->GetDirectoryEntryStream(&cc, "0E03");

    return cc;
}

std::string BlindCarbonCopy(std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string bcc = "";
    cfb->GetDirectoryEntryStream(&bcc, "0E02");

    return bcc;
}

std::string Subject(std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string subj = "";
    cfb->GetDirectoryEntryStream(&subj, "0070");
    if(subj.empty())
        cfb->GetDirectoryEntryStream(&subj, "0E1D");
    if(subj.empty())
        cfb->GetDirectoryEntryStream(&subj, "0037");

    return subj;
}

std::string Body(std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string body = "";
    cfb->GetDirectoryEntryStream(&body, "1000");

    return body;
}

std::string TransportHeader(std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string transportheader = "";
    cfb->GetDirectoryEntryStream(&transportheader, "007D");

    return transportheader;
}

uint8_t* substr(uint8_t* arr, int begin, int len)
{
    uint8_t* res = new uint8_t[len + 1];
    for (int i = 0; i < len; i++)
        res[i] = *(arr + begin + i);
    res[len] = 0;
    return res;
}

void ReadInteger(uint8_t* arr, int begin, uint16_t* val, bool isbigendian)
{
    uint8_t* tmp8 = new uint8_t[2];
    tmp8 = substr(arr, begin, 2);
    ReturnUint16(val, tmp8, isbigendian);
    delete[] tmp8;
}

void ReadInteger(uint8_t* arr, int begin, uint32_t* val, bool isbigendian)
{
    uint8_t* tmp8 = new uint8_t[4];
    tmp8 = substr(arr, begin, 4);
    ReturnUint32(val, tmp8, isbigendian);
    delete[] tmp8;
}

void ReadInteger(uint8_t* arr, int begin, uint64_t* val, bool isbigendian)
{
    uint8_t* tmp8 = new uint8_t[8];
    tmp8 = substr(arr, begin, 8);
    ReturnUint64(val, tmp8, isbigendian);
    delete[] tmp8;
}

void ReturnUint32(uint32_t* tmp32, uint8_t* tmp8, bool isbigendian)
{
    if(isbigendian)
        *tmp32 = __builtin_bswap32((uint32_t)tmp8[0] | (uint32_t)tmp8[1] << 8 | (uint32_t)tmp8[2] << 16 | (uint32_t)tmp8[3] << 24);
    else
        *tmp32 = (uint32_t)tmp8[0] | (uint32_t)tmp8[1] << 8 | (uint32_t)tmp8[2] << 16 | (uint32_t)tmp8[3] << 24;
}

void ReturnUint16(uint16_t* tmp16, uint8_t* tmp8, bool isbigendian)
{
    if(isbigendian)
        *tmp16 = __builtin_bswap16((uint16_t)tmp8[0] | (uint16_t)tmp8[1] << 8);
    else
        *tmp16 = (uint16_t)tmp8[0] | (uint16_t)tmp8[1] << 8;
}

void ReturnUint64(uint64_t* tmp64, uint8_t* tmp8, bool isbigendian)
{
    if(isbigendian)
        *tmp64 = __builtin_bswap64((uint64_t)tmp8[0] | (uint64_t)tmp8[1] << 8 | (uint64_t)tmp8[2] << 16 | (uint64_t)tmp8[3] << 24 | (uint64_t)tmp8[4] << 32 | (uint64_t)tmp8[5] << 40 | (uint64_t)tmp8[6] << 48 | (uint64_t)tmp8[7] << 56);
    else
        *tmp64 = (uint64_t)tmp8[0] | (uint64_t)tmp8[1] << 8 | (uint64_t)tmp8[2] << 16 | (uint64_t)tmp8[3] << 24 | (uint64_t)tmp8[4] << 32 | (uint64_t)tmp8[5] << 40 | (uint64_t)tmp8[6] << 48 | (uint64_t)tmp8[7] << 56;
}

std::string ConvertWindowsTimeToUnixTimeUTC(uint64_t input)
{
    uint64_t temp;
    temp = input / TICKS_PER_SECOND; //convert from 100ns intervals to seconds;
    temp = temp - EPOCH_DIFFERENCE;  //subtract number of seconds between epochs
    time_t crtimet = (time_t)temp;
    struct tm* dt;
    dt = gmtime(&crtimet);
    char timestr[30];
    strftime(timestr, sizeof(timestr), "%m/%d/%Y %I:%M:%S %p", dt);

    return timestr;
}

std::string Date(std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string date = "";
    uint8_t* propertybuffer = NULL;
    uint64_t pbsize = 0;
    cfb->GetDirectoryEntryBuffer(&propertybuffer, &pbsize, "__properties_version1.0");
    uint32_t propertycount = (pbsize - 32) / 16;
    // LOOP OVER PROPERTIES TO FIND THE TIMES
    uint64_t cursize = pbsize - 32;
    int curoffset = 32;
    for(int i=0; i < propertycount; i++)
    {
        curoffset = 32 + (i * 16);
        uint16_t ptype = 0;
        ReadInteger(propertybuffer, curoffset, &ptype); 
        if(date.empty())
        {
            if(ptype == 0x0040)
            {
                uint16_t dtype = 0;
                ReadInteger(propertybuffer, curoffset + 2, &dtype);
                if(dtype == 0x0010 || dtype == 0x0e06 || dtype == 0x3008 || dtype == 0x3007 || dtype == 0x0039)
                {
                    uint64_t filetime = 0;
                    ReadInteger(propertybuffer, curoffset + 8, &filetime);
                    date = ConvertWindowsTimeToUnixTimeUTC(filetime);
                }
            }
        }
        else if(!date.empty())
            break;
    }
    //std::cout << "properties: " << date.at(0) << std::endl;
    return date;
}

void AttachmentCount(uint32_t* attachcount, std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    uint8_t* propertybuffer = NULL;
    uint64_t pbsize = 0;
    cfb->GetDirectoryEntryBuffer(&propertybuffer, &pbsize, "__properties_version1.0");
    ReadInteger(propertybuffer, 20, attachcount);
}

void GetMsgAttachments(std::vector<AttachmentInfo>* msgattachments, uint32_t attachcount, std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    for(uint32_t i=0; i < attachcount; i++)
    {
        std::stringstream strm;
        strm << std::hex << i;
        std::string attachstr = "";
        if(i < 0x10)
            attachstr = "__attach_version1.0_#0000000" + strm.str();
        else if(i >= 0x10 && i < 0x100)
            attachstr = "__attach_version1.0_#000000" + strm.str();
        else
            attachstr = "__attach_version1.0_#00000" + strm.str();
        //std::cout << "attachment entry: " << attachstr << std::endl;
        DirectoryEntry parentry;
        cfb->NavigateDirectoryTree(&parentry, attachstr, 0);
        //std::cout << parentry.name << " parentry id: " << parentry.id << " cid: " << parentry.childid << std::endl;
        DirectoryEntry curentry;
        uint32_t curid = parentry.childid;
        AttachmentInfo curattachinfo;
        while(curentry.rightsiblingid < 0xffffffff)
        {
            //curattachinfo.id = i;
            cfb->GetDirectoryEntry(&curentry, curid);
            if(curentry.name.find("3701") != std::string::npos) // Binary Data
            {
                curattachinfo.dataid = curentry.id;
                //std::cout << "data id:" << curentry.id << "|" << curid << std::endl;
            }
            else if(curentry.name.find("3704") != std::string::npos) // Name
            {
                cfb->GetEntryStream(&curentry, &(curattachinfo.name));
                //void GetEntryStream(DirectoryEntry* currententry, std::string* entrystream);
                //std::cout << "get short name" << std::endl;
            }
            else if(curentry.name.find("3707") != std::string::npos) // Long Name
            {
                cfb->GetEntryStream(&curentry, &(curattachinfo.longname));
                //std::cout << "get long name" << std::endl;
            }
            else if(curentry.name.find("370E") != std::string::npos) // Mime Type
            {
                cfb->GetEntryStream(&curentry, &(curattachinfo.mimetag));
                //std::cout << "get mime tag" << std::endl;
            }
            else if(curentry.name.find("3712") != std::string::npos) // Content Id
            {
                cfb->GetEntryStream(&curentry, &(curattachinfo.contentid));
                //std::cout << "get content id" << std::endl;
            }
            //std::cout << curentry.name << " nextid: " << curentry.rightsiblingid << std::endl;
            curid = curentry.rightsiblingid;
        }
        msgattachments->push_back(curattachinfo);
    }
}
*/
