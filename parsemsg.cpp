#include "parsemsg.h"

std::string SenderName(std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string sendername = "";
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
