#include "parsemsg.h"

//std::string ParseMsg::SenderName(void)
std::string SenderName(std::string* mailboxpath)
{
    CompoundFileBinary* cfb = new CompoundFileBinary(mailboxpath);
    cfb->NavigateDirectoryEntries();
    std::string sendername = "";
    //FindDirectoryEntry("0C1A");
    cfb->GetDirectoryEntryStream(&sendername, "0C1A");
    if(sendername.empty())
        cfb->GetDirectoryEntryStream(&sendername, "3FFA");
    if(sendername.empty())
        cfb->GetDirectoryEntryStream(&sendername, "0042");
    //std::cout << "sender name: " << sendername << std::endl;
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
