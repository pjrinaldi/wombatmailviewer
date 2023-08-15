#include "parsemsg.h"

std::string SenderName(std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string sendername = "";
    DirectoryEntry curentry;
    // need to move the navigatedirectorytree into getdirectoryentrystream
    cfb->NavigateDirectoryTree(&curentry, "0C1A", 0);
    cfb->GetDirectoryEntryStream(&curentry, &sendername);
    std::cout << "sender name: " << sendername << std::endl;
    //cfb->GetDirectoryEntryStream(&sendername, "0C1A");
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
    if(senderaddress.empty())
        cfb->GetDirectoryEntryStream(&senderaddress, "5D01");
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

std::string Date(std::string* mailboxpath)
{
    return "";
    /*
    //m_date = getDateTimeFromStream("__properties_version1.0");
    std::stringstream ss;
    POLE::Stream requested_stream(m_File, stream);
    if (requested_stream.fail())
        std::cout << "Failed to obtain stream: " << stream << '\n';
    else {
        uint64_t microt = 0;
        uint32_t address = 0;
        int read;
        do {
            read = requested_stream.read(reinterpret_cast<unsigned char*>(&address), 4);
        } while (read > 0 && address != 0x00390040u && address != 0x0E060040u && address != 0x80080040u);

        //std::cout << "microt: " << microt << " address: " << address << std::endl;

        if (address == 0x00390040u || address == 0x0E060040u || address == 0x80080040u) {
            requested_stream.read(reinterpret_cast<unsigned char*>(&address), 4);
            requested_stream.read(reinterpret_cast<unsigned char*>(&microt), 8);
        
            //std::cout << "microt: " << microt << " address: " << address << std::endl;

            microt = microt / 10;

            std::tm t;
            t.tm_year = 1601 - 1900;
            t.tm_mday = 1;
            t.tm_mon  = 0;
            t.tm_hour = 0;
            t.tm_min  = 0;
            t.tm_sec  = 0;

            std::time_t pTypTime = std::mktime(&t); // PTypTime() ==
                                                    // 01/01/1601
                                                    // 00:00:00.

            std::chrono::system_clock::time_point tpDate =
                (std::chrono::system_clock::from_time_t(pTypTime) + std::chrono::microseconds(microt));
            std::time_t ttDate = std::chrono::system_clock::to_time_t(tpDate);

            t = *(std::localtime(&ttDate));

            //ss << std::setw(4) << std::setfill('0') << t.tm_year + 1900 << '-' << std::setw(2)
               //<< std::setfill('0') << t.tm_mon + 1 << '-' << std::setw(2) << std::setfill('0') << t.tm_mday
            ss << std::setw(2) << std::setfill('0') << t.tm_mon + 1 << "/" << std::setw(2) << std::setfill('0')
               << t.tm_mday << "/" << std::setw(4) << std::setfill('0') << t.tm_year + 1900
               << " " << std::setw(2) << std::setfill('0') << t.tm_hour << ":" << std::setw(2) << std::setfill('0')
               << t.tm_min << ":" << std::setw(2) << std::setfill('0') << t.tm_sec << " UTC";
        }
    }
    return ss.str();
    */
}
