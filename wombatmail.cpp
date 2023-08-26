#include "wombatmail.h"

// Copyright 2013-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

FXIMPLEMENT(WombatMail,FXMainWindow,WombatMailMap,ARRAYNUMBER(WombatMailMap))

WombatMail::WombatMail(FXApp* a):FXMainWindow(a, "Wombat Mail Forensics", new FXICOIcon(a, wombat_32), new FXICOIcon(a, wombat_32), DECOR_ALL, 0, 0, 1024, 768)
{
    new FXToolTip(this->getApp(), TOOLTIP_PERMANENT);
    mainframe = new FXVerticalFrame(this, LAYOUT_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
    toolbar = new FXToolBar(mainframe, this, LAYOUT_TOP|LAYOUT_LEFT);
    vsplitter = new FXSplitter(mainframe, SPLITTER_NORMAL|LAYOUT_FILL);
    statusbar = new FXStatusBar(mainframe, LAYOUT_BOTTOM|LAYOUT_LEFT|LAYOUT_FILL_X);
    treelist = new FXTreeList(vsplitter, this, ID_TREESELECT, TREELIST_SHOWS_LINES|TREELIST_SINGLESELECT|TREELIST_ROOT_BOXES|TREELIST_SHOWS_BOXES);
    treelist->setWidth(this->getWidth() / 4);
    hsplitter = new FXSplitter(vsplitter, SPLITTER_VERTICAL);
    tablelist = new FXTable(hsplitter, this, ID_TABLESELECT, TABLE_COL_SIZABLE|LAYOUT_FILL_X, LAYOUT_FILL_Y);
    plainfont = new FXFont(a, "monospace");
    plaintext = new FXText(hsplitter);
    plaintext->setHeight(this->getHeight() / 2);
    attachmentlist = new FXList(hsplitter, this, ID_LISTSELECT, LIST_SINGLESELECT|LAYOUT_LEFT|LAYOUT_FILL_X);
    attachmentlist->setHeight(25);
    plaintext->setFont(plainfont);
    plaintext->setEditable(false);
    tablelist->setHeight(this->getHeight() / 3);
    tablelist->setEditable(false);
    tablelist->setTableSize(10, 5);
    tablelist->setColumnText(0, "ID");
    tablelist->setColumnText(1, "Tag");
    tablelist->setColumnText(2, "From");
    tablelist->setColumnText(3, "Date Time");
    tablelist->setColumnText(4, "Subject");
    tablelist->setColumnHeaderHeight(tablelist->getColumnHeaderHeight() + 5);
    tablelist->setRowHeaderWidth(0);
    openicon = new FXPNGIcon(this->getApp(), folderopen);
    openbutton = new FXButton(toolbar, "", openicon, this, ID_OPEN, BUTTON_TOOLBAR|FRAME_RAISED, 0,0,0,0, 4,4,4,4);
    openbutton->setTipText("Open Mail File");
    managetagsicon = new FXPNGIcon(this->getApp(), managetags);
    managetagsbutton = new FXButton(toolbar, "", managetagsicon, this, ID_MANAGETAGS, BUTTON_TOOLBAR|FRAME_RAISED, 0,0,0,0, 4,4,4,4);
    managetagsbutton->setTipText("Manage Tags");
    headericon = new FXPNGIcon(this->getApp(), mail);
    headerbutton = new FXButton(toolbar, "", headericon, this, ID_HEADER, BUTTON_TOOLBAR|FRAME_RAISED, 0,0,0,0, 4,4,4,4);
    headerbutton->setTipText("Show Header");
    headerbutton->hide();
    previewicon = new FXPNGIcon(this->getApp(), reportpreview1);
    previewbutton = new FXButton(toolbar, "", previewicon, this, ID_PREVIEW, BUTTON_TOOLBAR|FRAME_RAISED, 0,0,0,0, 4,4,4,4);
    previewbutton->setTipText("Preview Report");
    publishicon = new FXPNGIcon(this->getApp(), paperairplane2);
    publishbutton = new FXButton(toolbar, "", publishicon, this, ID_PUBLISH, BUTTON_TOOLBAR|FRAME_RAISED, 0,0,0,0, 4,4,4,4);
    publishbutton->setTipText("Publish Report");
    abouticon = new FXPNGIcon(this->getApp(), helpcontents);
    aboutbutton = new FXButton(toolbar, "", abouticon, this, ID_ABOUT, BUTTON_TOOLBAR|FRAME_RAISED, 0,0,0,0, 4,4,4,4);
    aboutbutton->setTipText("About Wombat Mail Forensics");
    statusbar->getStatusLine()->setNormalText("Open a Mail Item to Begin");
    tags.clear();
    taggedlist.clear();
    ch = vmime::charset(vmime::charsets::UTF_8);
    attachmentlist->appendItem("No Attachments");
}

void WombatMail::create()
{
    FXMainWindow::create();
    show(PLACEMENT_SCREEN);
    if(this->getApp()->getArgc() == 2)
    {
        cmdmailboxpath = std::filesystem::canonical(std::filesystem::path(std::string(this->getApp()->getArgv()[1]))).string();
        int ret = OpenMailBox(NULL, 0, NULL);
    }
}

long WombatMail::ShowHeader(FXObject*, FXSelector, void* checkstate)
{

    return 1;
}
long WombatMail::TagMenu(FXObject*, FXSelector, void* ptr)
{
    FXEvent* event = (FXEvent*)ptr;
    if(tablelist->getCurrentRow() > -1 && !tablelist->getItemText(tablelist->getCurrentRow(), 0).empty())
    {
        if(!event->moved)
        {
            FXMenuPane tagmenu(this, POPUP_SHRINKWRAP);
            new FXMenuCommand(&tagmenu, "Create New Tag", new FXPNGIcon(this->getApp(), bookmarknew), this, ID_NEWTAG);
            new FXMenuSeparator(&tagmenu);
            for(int i=0; i < tags.size(); i++)
            {
                new FXMenuCommand(&tagmenu, FXString(tags.at(i).c_str()), new FXPNGIcon(this->getApp(), bookmark), this, ID_SETTAG);
            }
            new FXMenuSeparator(&tagmenu);
            new FXMenuCommand(&tagmenu, "Remove Tag", new FXPNGIcon(this->getApp(), bookmarkrem), this, ID_REMTAG);
            tagmenu.forceRefresh();
            tagmenu.create();
            tagmenu.popup(nullptr, event->root_x, event->root_y);
            getApp()->runModalWhileShown(&tagmenu);
        }
    }
    return 1;
}

long WombatMail::CreateNewTag(FXObject*, FXSelector, void*)
{
    FXString tagstr = "";
    bool isset = FXInputDialog::getString(tagstr, this, "Enter Tag Name", "New Tag");
    if(isset)
    {
        tags.push_back(tagstr.text());
        tablelist->setItemText(tablelist->getCurrentRow(), 1, tagstr);
	if(tagstr.length() > 5)
	    tablelist->fitColumnsToContents(1);
    }
    FXTreeItem* curitem = treelist->getCurrentItem();
    FXString rootstring = "";
    FXString mailboxpath = "";
    GetRootString(curitem, &rootstring);
    int found1 = rootstring.find(" (");
    int found2 = rootstring.find(")");
    mailboxpath = rootstring.mid(found1 + 2, found2 - found1 - 2) + rootstring.mid(0, found1);
    FXString idkeyvalue = mailboxpath + "\t" + curitem->getText() + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 2) + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 3) + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 4);
    //FXString idkeyvalue = mailboxpath + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 2) + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 3) + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 4);
    for(int i=0; i < taggedlist.no(); i++)
    {
        if(taggedlist.at(i).contains(idkeyvalue))
            taggedlist.erase(i);
    }
    taggedlist.append(tagstr + "|" + idkeyvalue + "|" + plaintext->getText());
    return 1;
}

long WombatMail::RemoveTag(FXObject*, FXSelector, void*)
{
    tablelist->setItemText(tablelist->getCurrentRow(), 1, "");
    FXTreeItem* curitem = treelist->getCurrentItem();
    FXString rootstring = "";
    FXString mailboxpath = "";
    GetRootString(curitem, &rootstring);
    int found1 = rootstring.find(" (");
    int found2 = rootstring.find(")");
    mailboxpath = rootstring.mid(found1 + 2, found2 - found1 - 2) + rootstring.mid(0, found1);
    FXString idkeyvalue = mailboxpath + "\t" + curitem->getText() + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 2) + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 3) + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 4);
    //FXString idkeyvalue = mailboxpath + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 2) + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 3) + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 4);
    for(int i=0; i < taggedlist.no(); i++)
    {
        if(taggedlist.at(i).contains(idkeyvalue))
            taggedlist.erase(i);
    }
    return 1;
}

void WombatMail::PopulatePstFolder(FXString mailboxpath, FXString curitemtext)
{
    tablelist->clearItems();
    tablelist->setTableSize(10, 5);
    tablelist->setColumnText(0, "ID");
    tablelist->setColumnText(1, "Tag");
    tablelist->setColumnText(2, "From");
    tablelist->setColumnText(3, "Date Time");
    tablelist->setColumnText(4, "Subject");
    int reterr = 0;
    libpff_error_t* pfferr = NULL;
    if(libpff_check_file_signature(mailboxpath.text(), &pfferr)) // is pst/ost
    {
        libpff_file_t* pffile = NULL;
        reterr = libpff_file_initialize(&pffile, &pfferr);
        reterr = libpff_file_open(pffile, mailboxpath.text(), LIBPFF_OPEN_READ, &pfferr);
	libpff_error_fprint(pfferr, stderr);
        libpff_item_t* rootfolder = NULL;
        reterr = libpff_file_get_root_folder(pffile, &rootfolder, &pfferr);
	libpff_error_fprint(pfferr, stderr);
        libpff_item_t* selectedfolder = NULL;
        libpff_item_t* tmpitem = NULL;
	tmpitem = rootfolder;
        std::string curitemstring = std::string(curitemtext.text());
	std::string curitemindex = "";
	std::map<std::string, std::string>::const_iterator keyval = foldermap.find(curitemstring);
	if(keyval == foldermap.end())
	{
	    std::cout << curitemstring << " not in the map, but should be.." << std::endl;
	}
	else
	    curitemindex = keyval->second;
	std::vector<std::string> indexlist;
	indexlist.clear();
	std::istringstream indexliststream(curitemindex);
	std::string curindex;
	while(getline(indexliststream, curindex, ','))
	    indexlist.push_back(curindex);
	for(int i = 0; i < indexlist.size(); i++)
	{
	    libpff_item_t* curitem = NULL;
	    reterr = libpff_folder_get_sub_folder(tmpitem, std::stoi(indexlist.at(i)), &curitem, &pfferr);
	    if(i < indexlist.size() - 1)
	    {
		tmpitem = curitem;
		curitem = NULL;
	    }
	    else
	    {
		selectedfolder = curitem;
		tmpitem = NULL;
	    }
	}
        int msgcnt = 0;
        reterr = libpff_folder_get_number_of_sub_messages(selectedfolder, &msgcnt, &pfferr);
        if(msgcnt > 0)
        {
	    tablelist->clearItems();
            tablelist->setTableSize(msgcnt, 5);
            tablelist->setColumnText(0, "ID");
            tablelist->setColumnText(1, "Tag");
            tablelist->setColumnText(2, "From");
            tablelist->setColumnText(3, "Date Time");
            tablelist->setColumnText(4, "Subject");
            for(int i=0; i < msgcnt; i++)
            {
                libpff_item_t* curmsg = NULL;
                reterr = libpff_folder_get_sub_message(selectedfolder, i, &curmsg, &pfferr);
                size_t msgsubjectsize = 0;
                reterr = libpff_message_get_entry_value_utf8_string_size(curmsg, LIBPFF_ENTRY_TYPE_MESSAGE_SUBJECT, &msgsubjectsize, &pfferr);
                uint8_t msgsubject[msgsubjectsize];
                reterr = libpff_message_get_entry_value_utf8_string(curmsg, LIBPFF_ENTRY_TYPE_MESSAGE_SUBJECT, msgsubject, msgsubjectsize, &pfferr);
                // REMOVES ^A AND REPLACES WITH SPACE
                for(int j=0; j < msgsubjectsize; j++)
                {
                    if((unsigned int)msgsubject[j] == 0x01)
                        msgsubject[j] = ' ';
                }
                size_t msgsendersize = 0;
                reterr = libpff_message_get_entry_value_utf8_string_size(curmsg, LIBPFF_ENTRY_TYPE_MESSAGE_SENDER_EMAIL_ADDRESS, &msgsendersize, &pfferr);
                uint8_t msgsender[msgsendersize];
                reterr = libpff_message_get_entry_value_utf8_string(curmsg, LIBPFF_ENTRY_TYPE_MESSAGE_SENDER_EMAIL_ADDRESS, msgsender, msgsendersize, &pfferr);

                uint64_t msgtime = 0;
                reterr = libpff_message_get_delivery_time(curmsg, &msgtime, &pfferr);
                if(reterr < 1)
                {
                    reterr = libpff_message_get_client_submit_time(curmsg, &msgtime, &pfferr);
                    if(reterr < 1)
                    {
                        reterr = libpff_message_get_creation_time(curmsg, &msgtime, &pfferr);
                        if(reterr < 1)
                        {
                            reterr = libpff_message_get_modification_time(curmsg, &msgtime, &pfferr);
                        }
                    }
                }
                tablelist->setItemText(i, 0, FXString::value(i+1));
                FXString tagstr = "";
                for(int j = 0; j < taggedlist.no(); j++)
                {
                    if(taggedlist.at(j).contains(mailboxpath) && taggedlist.at(j).contains(treelist->getCurrentItem()->getText()))
                    {
                        std::size_t found = taggedlist.at(j).find("|");
                        tagstr = taggedlist.at(j).left(found);
                    }
                }
                tablelist->setItemText(i, 1, tagstr);
                //std::cout << "|" << (unsigned int)msgsubject[0] << "|" << std::endl;
                if(!tagstr.empty() && tagstr.length() > 5)
                    tablelist->fitColumnsToContents(1);
                tablelist->setItemText(i, 2, FXString(reinterpret_cast<char*>(msgsender)));
                tablelist->setItemText(i, 3, ConvertWindowsTimeToUnixTimeUTC(msgtime));
                tablelist->setItemText(i, 4, FXString(reinterpret_cast<char*>(msgsubject)));
            }
	    tablelist->fitColumnsToContents(2);
	    tablelist->fitColumnsToContents(3);
	    tablelist->fitColumnsToContents(4);
        }
        reterr = libpff_item_free(&selectedfolder, &pfferr);
	reterr = libpff_item_free(&rootfolder, &pfferr);
        reterr = libpff_file_close(pffile, &pfferr);
        reterr = libpff_file_free(&pffile, &pfferr);
	AlignColumn(tablelist, 0, FXTableItem::LEFT);
	AlignColumn(tablelist, 1, FXTableItem::LEFT);
	AlignColumn(tablelist, 2, FXTableItem::LEFT);
	AlignColumn(tablelist, 3, FXTableItem::LEFT);
	AlignColumn(tablelist, 4, FXTableItem::LEFT);
	/*
	tablelist->setCurrentItem(0, 0);
	tablelist->selectRow(0, true);
	*/
    }
    libpff_error_free(&pfferr);
}

long WombatMail::MailBoxSelected(FXObject* sender, FXSelector, void*)
{
    plaintext->setText("");
    FXTreeItem* curitem = treelist->getCurrentItem();
    FXString rootstring = "";
    FXString mailboxpath = "";
    GetRootString(curitem, &rootstring);
    int found1 = rootstring.find(" (");
    int found2 = rootstring.find(")");
    mailboxpath = rootstring.mid(found1 + 2, found2 - found1 - 2) + rootstring.mid(0, found1);
    uint8_t mailboxtype = MailBoxType(mailboxpath.text());
    if(mailboxtype == 0x01) // PST/OST
    {
        PopulatePstFolder(mailboxpath, curitem->getText());
    }
    else if(mailboxtype == 0x02) // MBOX
    {
	PopulateMbox(mailboxpath.text());
    }
    else if(mailboxtype == 0x03) // MSG
    {
	PopulateMsg(mailboxpath.text());
    }
    else if(mailboxtype == 0x04) // EML
    {
	PopulateEml(mailboxpath.text());
    }

    return 1;
}

void WombatMail::GetRootString(FXTreeItem* curitem, FXString* rootstring)
{
    if(curitem->getParent() == NULL)
	*rootstring = curitem->getText();
    else
        GetRootString(curitem->getParent(), rootstring);
}

FXString WombatMail::ConvertUnixTimeToString(uint32_t input)
{
    time_t crtimet = (time_t)input;
    struct tm* dt;
    dt = gmtime(&crtimet);
    char timestr[30];
    strftime(timestr, sizeof(timestr), "%m/%d/%Y %I:%M:%S %p", dt);

    return timestr;
}

FXString WombatMail::ConvertWindowsTimeToUnixTimeUTC(uint64_t input)
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

long WombatMail::MessageSelected(FXObject*, FXSelector, void*)
{
    plaintext->setText("");
    if(tablelist->getCurrentRow() > -1)
    {
	tablelist->selectRow(tablelist->getCurrentRow());
	FXTreeItem* curitem = treelist->getCurrentItem();
	FXString rootstring = "";
	FXString mailboxpath = "";
	GetRootString(curitem, &rootstring);
	int found1 = rootstring.find(" (");
	int found2 = rootstring.find(")");
	mailboxpath = rootstring.mid(found1 + 2, found2 - found1 - 2) + rootstring.mid(0, found1);
	uint8_t mailboxtype = MailBoxType(mailboxpath.text());
	if(mailboxtype == 0x01) // PST/OST
	    PopulatePstEmail(mailboxpath, curitem->getText());
	else if(mailboxtype == 0x02) // MBOX
	    PopulateMboxEmail(mailboxpath, curitem->getText());
    }
    
    return 1;
}

long WombatMail::AttachmentSelected(FXObject*, FXSelector, void*)
{
    // save file content to tmp
    FXTreeItem* curitem = treelist->getCurrentItem();
    FXString rootstring = "";
    FXString mailboxpath = "";
    GetRootString(curitem, &rootstring);
    int found1 = rootstring.find(" (");
    int found2 = rootstring.find(")");
    mailboxpath = rootstring.mid(found1 + 2, found2 - found1 - 2) + rootstring.mid(0, found1);
    uint8_t mailboxtype = 0x00;
    mailboxtype = MailBoxType(mailboxpath.text());
    int curitemid = attachmentlist->getCurrentItem();
    if(mailboxtype == 0x03) // MSG
    {
        if(msgattachments.size() > 0)
        {
            std::string mbpath(mailboxpath.text());
            OutlookMessage* amsg = new OutlookMessage(&mbpath);
            std::vector<uint8_t> attachmentcontent;
            amsg->InitializeMessage();
            amsg->GetAttachmentContent(&attachmentcontent, msgattachments.at(curitemid).dataid);
            FXString tmpfilename = "/tmp/" + attachmentlist->getItemText(curitemid);
            FXString buf = "";
            for(int i=0; i < attachmentcontent.size(); i++)
                buf.append(attachmentcontent.at(i));
            FXFile* tmpfile = new FXFile(tmpfilename, FXIO::Writing, FXIO::OwnerReadWrite);
            tmpfile->writeBlock(buf.text(), buf.length());
            tmpfile->close();
            std::string defaultopen = "xdg-open " + std::string(tmpfilename.text()) + " &";
            std::system(defaultopen.c_str());
        }
    }
    else if(mailboxtype == 0x02 || mailboxtype == 0x04) // MBOX || EML
    {
        FILE* mailboxfile = fopen(mailboxpath.text(), "r");
        struct stat sb{};
        std::string msg;
        stat(mailboxpath.text(), &sb);
        msg.resize(sb.st_size);
        fread(const_cast<char*>(msg.data()), sb.st_size, 1, mailboxfile);
        fclose(mailboxfile);
        vmime::string msgdata = msg;
        vmime::shared_ptr<vmime::message> vmsg = vmime::make_shared<vmime::message>();
        vmsg->parse(msgdata);
        vmime::messageParser vparser(vmsg);
        const vmime::attachment& att = *vparser.getAttachmentAt(curitemid);
        vmime::byteArray ba;
        vmime::utility::outputStreamByteArrayAdapter bastrm(ba);
        att.getData()->extract(bastrm);
        FXString tmpfilename = "/tmp/" + attachmentlist->getItemText(curitemid);
        FXString buf = "";
        for(int i=0; i < ba.size(); i++)
            buf.append(ba.at(i));
        FXFile* tmpfile = new FXFile(tmpfilename, FXIO::Writing, FXIO::OwnerReadWrite);
        tmpfile->writeBlock(buf.text(), buf.length());
        tmpfile->close();
        std::string defaultopen = "xdg-open " + std::string(tmpfilename.text()) + " &";
        std::system(defaultopen.c_str());
    }
    else if(mailboxtype == 0x01) // PST/OST
    {
        int msgid = tablelist->getItemText(tablelist->getCurrentRow(), 0).toInt() - 1;
        int reterr = 0;
        libpff_error_t* pfferr = NULL;
        if(libpff_check_file_signature(mailboxpath.text(), &pfferr)) // is pst/ost
        {
            libpff_file_t* pffile = NULL;
            reterr = libpff_file_initialize(&pffile, &pfferr);
            reterr = libpff_file_open(pffile, mailboxpath.text(), LIBPFF_OPEN_READ, &pfferr);
            libpff_error_fprint(pfferr, stderr);
            libpff_item_t* rootfolder = NULL;
            reterr = libpff_file_get_root_folder(pffile, &rootfolder, &pfferr);
            libpff_error_fprint(pfferr, stderr);
            libpff_item_t* selectedfolder = NULL;
            libpff_item_t* tmpitem = NULL;
            tmpitem = rootfolder;
            std::string curitemstring = std::string(curitem->getText().text());
            std::string curitemindex = "";
            std::map<std::string, std::string>::const_iterator keyval = foldermap.find(curitemstring);
            if(keyval == foldermap.end())
            {
                std::cout << curitemstring << " not in the map, but should be.." << std::endl;
            }
            else
                curitemindex = keyval->second;
            std::vector<std::string> indexlist;
            indexlist.clear();
            std::istringstream indexliststream(curitemindex);
            std::string curindex;
            while(getline(indexliststream, curindex, ','))
                indexlist.push_back(curindex);
            for(int i = 0; i < indexlist.size(); i++)
            {
                libpff_item_t* curitem = NULL;
                reterr = libpff_folder_get_sub_folder(tmpitem, std::stoi(indexlist.at(i)), &curitem, &pfferr);
                if(i < indexlist.size() - 1)
                {
                    tmpitem = curitem;
                    curitem = NULL;
                }
                else
                {
                    selectedfolder = curitem;
                    tmpitem = NULL;
                }
            }
            libpff_item_t* curmsg = NULL;
            reterr = libpff_folder_get_sub_message(selectedfolder, msgid, &curmsg, &pfferr);
            libpff_item_t* curattach = NULL;
            reterr = libpff_message_get_attachment(curmsg, curitemid, &curattach, &pfferr);
            size_t attachnamesize = 0;
            reterr = libpff_message_get_entry_value_utf8_string_size(curattach, LIBPFF_ENTRY_TYPE_ATTACHMENT_FILENAME_LONG, &attachnamesize, &pfferr);
            uint8_t attachname[attachnamesize];
            reterr = libpff_message_get_entry_value_utf8_string(curattach, LIBPFF_ENTRY_TYPE_ATTACHMENT_FILENAME_LONG, attachname, attachnamesize, &pfferr);
            size64_t attachsize = 0;
            reterr = libpff_attachment_get_data_size(curattach, &attachsize, &pfferr);
            uint8_t attachbuf[attachsize];
            ssize_t bufread = 0;
            bufread = libpff_attachment_data_read_buffer(curattach, attachbuf, attachsize, &pfferr);
            FXString tmpfilename = "/tmp/" + FXString(reinterpret_cast<char*>(attachname));
            FXString buf = "";
            for(int i=0; i < attachsize; i++)
                buf.append((char)attachbuf[i]);
            FXFile* tmpfile = new FXFile(tmpfilename, FXIO::Writing, FXIO::OwnerReadWrite);
            tmpfile->writeBlock(buf.text(), buf.length());
            tmpfile->close();
            std::string defaultopen = "xdg-open \"" + std::string(tmpfilename.text()) + "\" &";
            std::system(defaultopen.c_str());
            reterr = libpff_item_free(&curmsg, &pfferr);
            reterr = libpff_item_free(&selectedfolder, &pfferr);
            reterr = libpff_item_free(&rootfolder, &pfferr);
            reterr = libpff_file_close(pffile, &pfferr);
            reterr = libpff_file_free(&pffile, &pfferr);
        }
        libpff_error_free(&pfferr);
    }

    return 1;
}

void WombatMail::PopulatePstEmail(FXString mailboxpath, FXString curitemtext)
{
    int msgid = tablelist->getItemText(tablelist->getCurrentRow(), 0).toInt() - 1;
    int reterr = 0;
    libpff_error_t* pfferr = NULL;
    if(libpff_check_file_signature(mailboxpath.text(), &pfferr)) // is pst/ost
    {
        libpff_file_t* pffile = NULL;
        reterr = libpff_file_initialize(&pffile, &pfferr);
        reterr = libpff_file_open(pffile, mailboxpath.text(), LIBPFF_OPEN_READ, &pfferr);
	libpff_error_fprint(pfferr, stderr);
        libpff_item_t* rootfolder = NULL;
        reterr = libpff_file_get_root_folder(pffile, &rootfolder, &pfferr);
	libpff_error_fprint(pfferr, stderr);
        libpff_item_t* selectedfolder = NULL;
        libpff_item_t* tmpitem = NULL;
	tmpitem = rootfolder;
        std::string curitemstring = std::string(curitemtext.text());
	std::string curitemindex = "";
	std::map<std::string, std::string>::const_iterator keyval = foldermap.find(curitemstring);
	if(keyval == foldermap.end())
	{
	    std::cout << curitemstring << " not in the map, but should be.." << std::endl;
	}
	else
	    curitemindex = keyval->second;
	std::vector<std::string> indexlist;
	indexlist.clear();
	std::istringstream indexliststream(curitemindex);
	std::string curindex;
	while(getline(indexliststream, curindex, ','))
	    indexlist.push_back(curindex);
	for(int i = 0; i < indexlist.size(); i++)
	{
	    libpff_item_t* curitem = NULL;
	    reterr = libpff_folder_get_sub_folder(tmpitem, std::stoi(indexlist.at(i)), &curitem, &pfferr);
	    if(i < indexlist.size() - 1)
	    {
		tmpitem = curitem;
		curitem = NULL;
	    }
	    else
	    {
		selectedfolder = curitem;
		tmpitem = NULL;
	    }
	}
        libpff_item_t* curmsg = NULL;
        reterr = libpff_folder_get_sub_message(selectedfolder, msgid, &curmsg, &pfferr);
	// get msg contents
	FXString msgbodystr = "";
	size_t msghdrsize = 0;
	reterr = libpff_message_get_entry_value_utf8_string_size(curmsg, LIBPFF_ENTRY_TYPE_MESSAGE_TRANSPORT_HEADERS, &msghdrsize, &pfferr);
	uint8_t msghdr[msghdrsize];
	reterr = libpff_message_get_entry_value_utf8_string(curmsg, LIBPFF_ENTRY_TYPE_MESSAGE_TRANSPORT_HEADERS, msghdr, msghdrsize, &pfferr);
	msgbodystr = FXString(reinterpret_cast<char*>(msghdr));
	size_t msgbodysize = 0;
	reterr = libpff_message_get_plain_text_body_size(curmsg, &msgbodysize, &pfferr);
	uint8_t msgbody[msgbodysize];
	msgbodystr += "\n\n";
	reterr = libpff_message_get_plain_text_body(curmsg, msgbody, msgbodysize, &pfferr);
	msgbodystr += FXString(reinterpret_cast<char*>(msgbody));
	plaintext->setText(msgbodystr.substitute('\r', ' '));
	// POPULATE ATTACHMENT PORTIONS
        attachmentlist->clearItems();
	int attachcnt = 0;
	reterr = libpff_message_get_number_of_attachments(curmsg, &attachcnt, &pfferr);
	libpff_error_fprint(pfferr, stderr);
	for(int i=0; i < attachcnt; i++)
	{
	    libpff_item_t* curattach = NULL;
	    reterr = libpff_message_get_attachment(curmsg, i, &curattach, &pfferr);
	    size_t attachnamesize = 0;
	    reterr = libpff_message_get_entry_value_utf8_string_size(curattach, LIBPFF_ENTRY_TYPE_ATTACHMENT_FILENAME_LONG, &attachnamesize, &pfferr);
	    uint8_t attachname[attachnamesize];
	    reterr = libpff_message_get_entry_value_utf8_string(curattach, LIBPFF_ENTRY_TYPE_ATTACHMENT_FILENAME_LONG, attachname, attachnamesize, &pfferr);
            attachmentlist->appendItem(FXString(reinterpret_cast<char*>(attachname)));
	}
	reterr = libpff_item_free(&curmsg, &pfferr);
	reterr = libpff_item_free(&selectedfolder, &pfferr);
	reterr = libpff_item_free(&rootfolder, &pfferr);
	reterr = libpff_file_close(pffile, &pfferr);
	reterr = libpff_file_free(&pffile, &pfferr);
    }
    libpff_error_free(&pfferr);
}

void WombatMail::GetMimeAttachments(std::string* msg)
{
    vmime::string msgdata = *msg;
    vmime::shared_ptr<vmime::message> vmsg = vmime::make_shared<vmime::message>();
    vmsg->parse(msgdata);
    vmime::messageParser vparser(vmsg);
    if(vparser.getAttachmentCount() > 0)
    {
	attachmentlist->clearItems();
	for(int i=0; i < vparser.getAttachmentCount(); i++)
	{
	    const vmime::attachment& att = *vparser.getAttachmentAt(i);
	    attachmentlist->appendItem(att.getName().getConvertedText(ch).c_str());
	}
    }
}

void WombatMail::PopulateEml(FXString mailboxpath)
{
    FILE* mailboxfile = fopen(mailboxpath.text(), "r");
    struct stat sb{};
    std::string msg;
    stat(mailboxpath.text(), &sb);
    msg.resize(sb.st_size);
    fread(const_cast<char*>(msg.data()), sb.st_size, 1, mailboxfile);
    fclose(mailboxfile);
    vmime::string msgdata = msg;
    vmime::shared_ptr<vmime::message> vmsg = vmime::make_shared<vmime::message>();
    vmsg->parse(msgdata);
    vmime::messageParser vparser(vmsg);
    std::string contents = "";
    GetMessageContent(&msg, &contents);
    plaintext->setText(FXString(contents.c_str()).substitute('\r', ' '));
    GetMimeAttachments(&msg);
}

void WombatMail::PopulateMboxEmail(FXString mailboxpath, FXString curitemtext)
{
    int msgid = tablelist->getItemText(tablelist->getCurrentRow(), 0).toInt() - 1;
    std::string contents = "";
    GetMessageContent(&(msgs.at(msgid)), &contents);
    plaintext->setText(FXString(contents.c_str()).substitute('\r', ' '));
    GetMimeAttachments(&(msgs.at(msgid)));
}

void WombatMail::GetMessageContent(std::string* msg, std::string* content)
{
    vmime::string msgdata = *msg;
    vmime::shared_ptr <vmime::message> vmsg = vmime::make_shared<vmime::message>();
    vmsg->parse(msgdata);
    vmime::messageParser vparser(vmsg);
    // MIME HEADER
    vmime::mailbox vfrom = vparser.getExpeditor();
    content->append("From:\t\t");
    content->append(vfrom.getName().getConvertedText(ch));
    content->append(" <");
    content->append(vfrom.getEmail().toString());
    content->append(">\n");
    vmime::addressList vtolist = vparser.getRecipients();
    for(int i=0; i < vtolist.getAddressCount(); i++)
    {
        vmime::shared_ptr<vmime::mailbox> curto = vmime::dynamicCast<vmime::mailbox>(vtolist.getAddressAt(i));
        content->append("To:\t\t");
        content->append(curto->getName().getConvertedText(ch));
        content->append(" <");
        content->append(curto->getEmail().toString());
        content->append(">\n");
    }
    vmime::addressList vcclist = vparser.getCopyRecipients();
    for(int i=0; i < vcclist.getAddressCount(); i++)
    {
        vmime::shared_ptr<vmime::mailbox> curcc = vmime::dynamicCast<vmime::mailbox>(vcclist.getAddressAt(i));
        content->append("Cc:\t\t");
        content->append(curcc->getName().getConvertedText(ch));
        content->append(" <");
        content->append(curcc->getEmail().toString());
        content->append(">\n");
    }
    vmime::addressList vbclist = vparser.getBlindCopyRecipients();
    for(int i=0; i < vbclist.getAddressCount(); i++)
    {
        vmime::shared_ptr<vmime::mailbox> curbc = vmime::dynamicCast<vmime::mailbox>(vbclist.getAddressAt(i));
        content->append("Bcc:\t\t");
        content->append(curbc->getName().getConvertedText(ch));
        content->append(" <");
        content->append(curbc->getEmail().toString());
        content->append(">\n");
    }
    vmime::text vsubj = vparser.getSubject();
    content->append("Subject:\t");
    content->append(vsubj.getConvertedText(ch));
    content->append("\n");
    vmime::datetime vdate = vparser.getDate();
    content->append("Date:\t\t");
    content->append(std::to_string(vdate.getMonth()));
    content->append("/");
    content->append(std::to_string(vdate.getDay()));
    content->append("/");
    content->append(std::to_string(vdate.getYear()));
    content->append(" ");
    content->append(std::to_string(vdate.getHour()));
    content->append(":");
    content->append(std::to_string(vdate.getMinute()));
    content->append(":");
    content->append(std::to_string(vdate.getSecond()));
    content->append("\n\n\n");
    // MIME BODY
    for(int i=0; i < vparser.getTextPartCount(); i++)
    {
        vmime::shared_ptr<const vmime::textPart> tp = vparser.getTextPartAt(i);
        if(tp->getType().getSubType() == vmime::mediaTypes::TEXT_HTML)
        {
            vmime::shared_ptr<const vmime::htmlTextPart> htp = vmime::dynamicCast<const vmime::htmlTextPart>(tp);
            vmime::string tstr;
            vmime::utility::outputStreamStringAdapter ostrm(tstr);
            htp->getPlainText()->extract(ostrm);
            content->append(tstr);
        }
        else
        {
            vmime::shared_ptr<const vmime::textPart> ttp = vmime::dynamicCast<const vmime::textPart>(tp);
            vmime::string tstr;
            vmime::utility::outputStreamStringAdapter ostrm(tstr);
            ttp->getText()->extract(ostrm);
            content->append(tstr);
        }
    }
}

void WombatMail::AlignColumn(FXTable* curtable, int col, FXuint justify)
{
    for(int i=0; i < curtable->getNumRows(); i++)
        curtable->setItemJustify(i, col, justify);
}

long WombatMail::OpenTagManager(FXObject*, FXSelector, void*)
{
    ManageTags tagmanager(this, "Manage Tags");
    tagmanager.SetTagList(&tags, &taggedlist);
    tagmanager.execute(PLACEMENT_OWNER);
    for(int i=0; i < tablelist->getNumRows(); i++)
    {
        FXTreeItem* curitem = treelist->getCurrentItem();
        FXString rootstring = "";
        FXString mailboxpath = "";
        GetRootString(curitem, &rootstring);
        int found1 = rootstring.find(" (");
        int found2 = rootstring.find(")");
        mailboxpath = rootstring.mid(found1 + 2, found2 - found1 - 2) + rootstring.mid(0, found1);
        FXString curtag = tablelist->getItemText(i, 0);
        for(int j=0; j < taggedlist.no(); j++)
        {
            std::size_t found = taggedlist.at(j).find("|");
            std::size_t rfound = taggedlist.at(j).rfind("|");
            FXString itemtag = taggedlist.at(j).mid(0, found);
            FXString itemhdr = taggedlist.at(j).mid(found+1, rfound - found - 1);
            if(itemhdr.contains(mailboxpath) && itemhdr.contains(curitem->getText()))
            {
                if(FXString::compare(curtag, itemtag) != 0)
                    tablelist->setItemText(i, 1, itemtag);
            }
        }
    }

    return 1;
}

long  WombatMail::OpenAboutBox(FXObject*, FXSelector, void*)
{
    AboutBox aboutbox(this, "About Wombat Mail Forensics");
    aboutbox.execute(PLACEMENT_OWNER);
    return 1;
}

long WombatMail::PreviewReport(FXObject*, FXSelector, void*)
{
    viewer = new Viewer(this, "Report Preview");
    viewer->GenerateReport(taggedlist, tags);
    viewer->execute(PLACEMENT_OWNER);

    return 1;
}

long WombatMail::PublishReport(FXObject*, FXSelector, void*)
{
    FXString startpath = FXString(getenv("HOME")) + "/";
    FXString filename = FXFileDialog::getSaveFilename(this, "Publish Report", startpath, "Text Files (*.txt)\nHTML Files (*.html,*.htm)");
    if(!filename.empty())
    {
        FXFile* outfile = new FXFile(filename, FXIO::Writing, FXIO::OwnerReadWrite);
        FXString buf;
        if(filename.contains(".htm"))
        {
            buf = "<html><head><title>Wombat Mail Report</title></head>\n";
            buf += "<body style='font-color: #3a291a; background-color: #d6ceb5;'>\n";
            buf += "<h2>Wombat Mail Report</h2>\n";
            buf += "<div id='toc'><h3>Contents</h3>\n";
            for(int j=0; j < tags.size(); j++)
            {
                int tagcnt = 0;
                for(int i=0; i < taggedlist.no(); i++)
                {
                    //if(taggedlist.at(i).contains(tags.at(j).c_str()))
                    std::size_t found = taggedlist.at(i).find("|");
                    FXString itemtag = taggedlist.at(i).mid(0, found);
                    if(FXString::compare(itemtag, FXString(tags.at(j).c_str())) == 0)
                        tagcnt++;
                }
                buf += "<div><a href='#t" + FXString::value(j) + "'>" + FXString(tags.at(j).c_str()) + " (" + FXString::value(tagcnt) + ")</a></div>\n";
            }
            buf += "<h3>Tagged Items</h3>";
            for(int i=0; i < tags.size(); i++)
            {
                buf += "<div id='t" + FXString::value(i) + "'><h4>" + tags.at(i).c_str() + "<span style='font-size: 10px;'>&nbsp;&nbsp;<a href='#toc'>TOP</a></span></h4>\n";
                for(int j=0; j < taggedlist.no(); j++)
                {
                    std::size_t found = taggedlist.at(j).find("|");
                    std::size_t rfound = taggedlist.at(j).rfind("|");
                    FXString itemtag = taggedlist.at(j).mid(0, found);
                    FXString itemhdr = taggedlist.at(j).mid(found+1, rfound - found - 1);
                    std::size_t hfind1 = itemhdr.find("\t");
                    std::size_t hfind2 = itemhdr.find("\t", hfind1+1);
                    std::size_t hfind3 = itemhdr.find("\t", hfind2+1);
                    std::size_t hfind4 = itemhdr.find("\t", hfind3+1);
                    FXString itemcon = taggedlist.at(j).mid(rfound+1, taggedlist.at(j).length() - rfound);
                    if(itemtag == tags.at(i).c_str())
                    {
                        buf += "<div style='border-bottom: 1px solid black;'>\n";
                        buf += "<div>Mail Item:&nbsp;&nbsp;&nbsp;&nbsp;" + itemhdr.mid(0, hfind1) + "</div>\n";
                        buf += "<div>Folder:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + itemhdr.mid(hfind1+1, hfind2 - hfind1 - 1) + "</div>\n";
                        buf += "<div>From:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + itemhdr.mid(hfind2+1, hfind3 - hfind2 - 1) + "</div>\n";
                        buf += "<div>Date:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + itemhdr.mid(hfind3+1, hfind4 - hfind3 - 1) + "</div>\n";
                        buf += "<div>Subject:&nbsp;&nbsp;&nbsp;&nbsp;" + itemhdr.mid(hfind4+1, taggedlist.at(j).length() - hfind4 - 1) + "</div>\n<br/>\n";
                        buf += "<div><pre>" + itemcon + "</pre></div>\n";
                        buf += "</div>\n";
                    }
                }
            }
            buf += "</body></html>";
        }
        else
        {
            viewer->GetText(&buf);
        }
        outfile->writeBlock(buf.text(), buf.length());
        outfile->close();
    }
    return 1;
}

long WombatMail::TableUp(FXObject*, FXSelector, void* ptr)
{
    int currow = tablelist->getCurrentRow();
    switch(((FXEvent*)ptr)->code)
    {
        case KEY_Up:
            tablelist->setCurrentItem(currow - 1, 0, true);
	    tablelist->selectRow(currow - 1, true);
            break;
        case KEY_Down:
            tablelist->setCurrentItem(currow + 1, 0, true);
	    tablelist->selectRow(currow + 1, true);
            break;
    }

    return 1;
}

long WombatMail::OpenMailBox(FXObject*, FXSelector, void*)
{
    FXString filename = "";
    if(cmdmailboxpath.empty())
    {
        if(oldmailboxpath.empty())
            oldmailboxpath = getenv("HOME") + std::string("/");
        filename = FXFileDialog::getOpenFilename(this, "Open MailBox", oldmailboxpath.c_str());
    }
    else
    {
        filename = FXString(cmdmailboxpath.c_str());
        cmdmailboxpath = "";
    }
    if(!filename.empty())
    {
	std::string mailboxpath = filename.text();
        // fix this to be path and not the path and the file...
        oldmailboxpath = mailboxpath;
	// check mailbox type
        uint8_t mailboxtype = 0x00;
	mailboxtype = MailBoxType(mailboxpath);
	if(mailboxtype == 0x01 || mailboxtype == 0x02 || mailboxtype == 0x03 || mailboxtype == 0x04)
	{
            std::size_t rfound = mailboxpath.rfind("/");
            std::string mailboxname = mailboxpath.substr(rfound+1);
            FXString rootitemstring(std::string(mailboxname + " (" + mailboxpath.substr(0, rfound+1) + ")").c_str());
            rootitem = new FXTreeItem(rootitemstring);
            treelist->appendItem(0, rootitem);
            if(mailboxtype == 0x01) // PST/OST
            {
                PopulatePst(mailboxpath);
            }
            else if(mailboxtype == 0x02) // MBOX
            {
                //PopulateMbox(mailboxpath);
            }
            else if(mailboxtype == 0x03) // MSG
            {
            }
	    else if(mailboxtype == 0x04) // EML
	    {
		//std::cout << "Parse EML HERE" << std::endl;
	    }
            StatusUpdate("Ready");
        }
        else // OTHER FILE, NOT SUPPORTED
        {
            StatusUpdate("File not opened. Not a valid mail item.");
        }
    }

    return 1;
}

uint8_t WombatMail::MailBoxType(std::string mailboxpath)
{
    msg = new OutlookMessage(&mailboxpath);
    uint8_t mailboxtype = 0x00;
    libpff_error_t* pfferr = NULL;
    if(libpff_check_file_signature(mailboxpath.c_str(), &pfferr)) // is pst/ost
	mailboxtype = 0x01; // PST/OST
    else if(msg->IsOutlookMessage())
        mailboxtype = 0x03; // MSG
    else // might be mbox or eml
    {
	std::regex mboxheader("^From .*[0-9][0-9]:[0-9][0-9]"); // kmbox regular expression
	std::ifstream mailboxfile(mailboxpath, std::ios::in|std::ios::binary);
	std::string line;
	while(std::getline(mailboxfile, line))
	{
	    std::smatch mboxmatch;
	    bool ismatch = std::regex_search(line, mboxmatch, mboxheader);
	    if(ismatch == true)
	    {
		mailboxtype = 0x02; // is mbox
		break;
	    }
	}
	if(mailboxtype != 0x02)
	{
	    FILE* mailboxfile = fopen(mailboxpath.c_str(), "r");
	    struct stat sb{};
	    std::string msg;
	    stat(mailboxpath.c_str(), &sb);
	    msg.resize(sb.st_size);
	    fread(const_cast<char*>(msg.data()), sb.st_size, 1, mailboxfile);
	    fclose(mailboxfile);
	    vmime::string msgdata = msg;
	    vmime::shared_ptr<vmime::message> vmsg = vmime::make_shared<vmime::message>();
	    vmsg->parse(msgdata);
	    vmime::messageParser vparser(vmsg);
	    if(vparser.getTextPartCount() > 0) // EML
	    {
		mailboxtype = 0x04; // is eml
	    }
	}
    }
    libpff_error_free(&pfferr);

    return mailboxtype;
}

void WombatMail::PopulateMsg(std::string mailboxpath)
{
    // my new method
    msg->InitializeMessage();
    std::string content = "";
    content.append("From:\t\t");
    content.append(msg->SenderName());
    content.append(" <");
    content.append(msg->SenderAddress());
    content.append(">\n");
    content.append("To:\t\t");
    content.append(msg->Receivers());
    content.append("\n");
    std::string ccstr = msg->CarbonCopy();
    if(!ccstr.empty())
    {
        content.append("Cc:\t\t");
        content.append(ccstr);
        content.append("\n");
    }
    std::string bccstr = msg->BlindCarbonCopy();
    if(!bccstr.empty())
    {
        content.append("Bcc:\t\t");
        content.append(bccstr);
        content.append("\n");
    }
    content.append("Subject:\t");
    content.append(msg->Subject());
    content.append("\n");
    content.append("Date:\t\t");
    content.append(msg->Date());
    content.append("\n");
    content.append("----------\n");
    content.append("\n");
    content.append(msg->Body());
    content.append("\n\n");
    content.append("----------\n");
    content.append(msg->TransportHeader());
    content.append("\n");
    plaintext->setText(FXString(content.c_str()).substitute('\r', ' '));
    // GET ATTACHMENT COUNT
    uint32_t attachcount = 0;
    msg->AttachmentCount(&attachcount);
    //std::cout << "Attachment Count: " << attachcount << std::endl;
    msgattachments.clear();
    msg->GetMsgAttachments(&msgattachments, attachcount);
    //std::cout << "attachment count: " << msgattachments.size() << std::endl;
    // POPULATE ATTACHMENTS
    attachmentlist->clearItems();
    for(int i=0; i < msgattachments.size(); i++)
    {
        if(!msgattachments.at(i).longname.empty())
            attachmentlist->appendItem(FXString(msgattachments.at(i).longname.c_str()));
        else if(!msgattachments.at(i).name.empty())
            attachmentlist->appendItem(FXString(msgattachments.at(i).name.c_str()));
        else if(!msgattachments.at(i).contentid.empty())
            attachmentlist->appendItem(FXString(msgattachments.at(i).contentid.c_str()));
    }
}

void WombatMail::PopulateMbox(std::string mailboxpath)
{
    std::string layout = "";
    std::vector<uint64_t> poslist;
    std::vector<uint64_t> linelength;
    poslist.clear();
    linelength.clear();
    std::regex mboxheader("^From .*[0-9][0-9]:[0-9][0-9]"); // kmbox regular expression
    std::ifstream mailboxfile(mailboxpath.c_str(), std::ios::in);
    std::string line;
    while(std::getline(mailboxfile, line))
    {
        std::smatch mboxmatch;
        bool ismatch = std::regex_search(line, mboxmatch, mboxheader);
        if(ismatch == true)
        {
            //std::cout << "mboxmatch pos: " << mailboxfile.tellg() << std::endl;
            poslist.push_back(mailboxfile.tellg());
            linelength.push_back(line.size());
            //std::cout << "line length: " << line.size() << std::endl;
        }
    }
    mailboxfile.close();
    mailboxfile.open(mailboxpath.c_str(), std::ios::in);
    mailboxfile.seekg(0, std::ifstream::beg);
    //std::cout << "mbf start: " << mailboxfile.tellg() << std::endl;
    mailboxfile.seekg(0, std::ifstream::end);
    //std::cout << "mailbox end: " << mailboxfile.tellg() << std::endl;
    poslist.push_back(mailboxfile.tellg());
    mailboxfile.close();
    tablelist->clearItems();
    tablelist->setTableSize(poslist.size() - 1, 5);
    tablelist->setColumnText(0, "ID");
    tablelist->setColumnText(1, "Tag");
    tablelist->setColumnText(2, "From");
    tablelist->setColumnText(3, "Date Time");
    tablelist->setColumnText(4, "Subject");
    msgs.clear();
    for(int i=0; i < poslist.size() - 1; i++)
    {
        int splitpos = 0;
        mailboxfile.open(mailboxpath.c_str(), std::ios::in|std::ios::binary);
        uint64_t pos = poslist.at(i);
        //std::cout << "pos: " << pos << " endpos: " << poslist.at(i+1) - poslist.at(i) - linelength.at(i) << std::endl;
        mailboxfile.seekg(poslist.at(i));
        while(pos <= poslist.at(i+1) - linelength.at(i))
        {
            std::string line = "";
            std::getline(mailboxfile, line);
            //std::cout << "line: " << line << std::endl;
            //if(line == "\n")
            if(line.compare("") == 0)
            {
                splitpos = mailboxfile.tellg() - (int64_t)poslist.at(i);
                //std::cout << "splitpos1: " << splitpos << std::endl;
                break;
            }
            pos = mailboxfile.tellg();
        }
        mailboxfile.seekg(poslist.at(i));
        char* mbuf = new char[poslist.at(i+1) - linelength.at(i) - poslist.at(i)];
        mailboxfile.read(mbuf, poslist.at(i+1) - linelength.at(i) - poslist.at(i));
        msgs.push_back(std::string(mbuf));
        mailboxfile.close();
    }
    for(int i=0; i < msgs.size(); i++)
    {
        std::string subjectstring = "";
        GetMimeSubject(&(msgs.at(i)), &subjectstring);
        std::string fromstring = "";
        GetMimeFrom(&(msgs.at(i)), &fromstring);
        std::string datestring = "";
        GetMimeDate(&(msgs.at(i)), &datestring);
        tablelist->setItemText(i, 0, FXString::value(i+1));
        //tablelist->setItemText(i, 1, "tagstr");
        tablelist->setItemText(i, 2, FXString(fromstring.c_str()));
        tablelist->setItemText(i, 3, FXString(datestring.c_str()));
        tablelist->setItemText(i, 4, FXString(subjectstring.c_str()));
    }
    AlignColumn(tablelist, 0, FXTableItem::LEFT);
    AlignColumn(tablelist, 1, FXTableItem::LEFT);
    AlignColumn(tablelist, 2, FXTableItem::LEFT);
    AlignColumn(tablelist, 3, FXTableItem::LEFT);
    AlignColumn(tablelist, 4, FXTableItem::LEFT);
    tablelist->fitColumnsToContents(2);
    tablelist->fitColumnsToContents(3);
    tablelist->fitColumnsToContents(4);
}

void WombatMail::GetMimeFrom(std::string* msg, std::string* from)
{
    vmime::string msgdata = *msg;
    vmime::shared_ptr<vmime::message> vmsg = vmime::make_shared<vmime::message>();
    vmsg->parse(msgdata);
    vmime::messageParser vparser(vmsg);
    vmime::mailbox vfrom = vparser.getExpeditor();
    from->append(vfrom.getName().getConvertedText(ch));
    from->append(" <");
    from->append(vfrom.getEmail().toString());
    from->append(">");
}

void WombatMail::GetMimeDate(std::string* msg, std::string* date)
{
    vmime::string msgdata = *msg;
    vmime::shared_ptr<vmime::message> vmsg = vmime::make_shared<vmime::message>();
    vmsg->parse(msgdata);
    vmime::messageParser vparser(vmsg);
    vmime::datetime vdate = vparser.getDate();
    date->append(std::to_string(vdate.getMonth()));
    date->append("/");
    date->append(std::to_string(vdate.getDay()));
    date->append("/");
    date->append(std::to_string(vdate.getYear()));
    date->append(" ");
    date->append(std::to_string(vdate.getHour()));
    date->append(":");
    date->append(std::to_string(vdate.getMinute()));
    date->append(":");
    date->append(std::to_string(vdate.getSecond()));
}

void WombatMail::GetMimeSubject(std::string* msg, std::string* subject)
{
    vmime::string msgdata = *msg;
    vmime::shared_ptr<vmime::message> vmsg = vmime::make_shared<vmime::message>();
    vmsg->parse(msgdata);
    vmime::messageParser vparser(vmsg);
    vmime::text vsubj = vparser.getSubject();
    subject->append(vsubj.getConvertedText(ch).c_str());
}

void WombatMail::PopulatePst(std::string mailboxpath)
{
    int reterr = 0;
    libpff_error_t* pfferr = NULL;
    if(libpff_check_file_signature(mailboxpath.c_str(), &pfferr)) // is pst/ost
    {
        // this is a pst/ost file, start processing it.
        libpff_file_t* pffile = NULL;
        reterr = libpff_file_initialize(&pffile, &pfferr);
        reterr = libpff_file_open(pffile, mailboxpath.c_str(), LIBPFF_OPEN_READ, &pfferr);
	libpff_error_fprint(pfferr, stderr);
        libpff_item_t* rootfolder = NULL;
        reterr = libpff_file_get_root_folder(pffile, &rootfolder, &pfferr);
	int subfoldercnt = 0;
	reterr = libpff_folder_get_number_of_sub_folders(rootfolder, &subfoldercnt, &pfferr);
	for(int i=0; i < subfoldercnt; i++)
	{
	    libpff_item_t* cursubfolder = NULL;
	    reterr = libpff_folder_get_sub_folder(rootfolder, i, &cursubfolder, &pfferr);
	    libpff_error_fprint(pfferr, stderr);
	    size_t subnamesize = 0;
	    reterr = libpff_folder_get_utf8_name_size(cursubfolder, &subnamesize, &pfferr);
	    libpff_error_fprint(pfferr, stderr);
	    uint8_t subname[subnamesize];
	    reterr = libpff_folder_get_utf8_name(cursubfolder, subname, subnamesize, &pfferr);
	    libpff_error_fprint(pfferr, stderr);
            FXTreeItem* subitem = new FXTreeItem(FXString(reinterpret_cast<char*>(subname)));
	    std::string subnamestr = "";
	    for(int j=0; j < subnamesize - 1; j++)
		subnamestr += (char)subname[j];
	    foldermap.insert(std::make_pair(subnamestr, std::to_string(i)));
            treelist->appendItem(rootitem, subitem);
	    int subdircnt = 0;
	    reterr = libpff_folder_get_number_of_sub_folders(cursubfolder, &subdircnt, &pfferr);
	    if(subdircnt > 0)
	    {
		PopulateSubFolders(mailboxpath, cursubfolder, subitem, std::to_string(i));
	    }
	    reterr = libpff_item_free(&cursubfolder, &pfferr);
	}
	reterr = libpff_item_free(&rootfolder, &pfferr);
        reterr = libpff_file_close(pffile, &pfferr);
        reterr = libpff_file_free(&pffile, &pfferr);
    }
    libpff_error_free(&pfferr);
}

void WombatMail::PopulateSubFolders(std::string mailboxpath, libpff_item_t* subfolder, FXTreeItem* subitem, std::string itemindex)
{
    int reterr = 0;
    libpff_error_t* pfferr = NULL;
    libpff_file_t* pffile = NULL;
    reterr = libpff_file_initialize(&pffile, &pfferr);
    reterr = libpff_file_open(pffile, mailboxpath.c_str(), LIBPFF_OPEN_READ, &pfferr);
    int childcnt = 0;
    reterr = libpff_folder_get_number_of_sub_folders(subfolder, &childcnt, &pfferr);
    for(int i=0; i < childcnt; i++)
    {
	libpff_item_t* childfolder = NULL;
	reterr = libpff_folder_get_sub_folder(subfolder, i, &childfolder, &pfferr);
	size_t childnamesize = 0;
	reterr = libpff_folder_get_utf8_name_size(childfolder, &childnamesize, &pfferr);
	uint8_t childname[childnamesize];
	reterr = libpff_folder_get_utf8_name(childfolder, childname, childnamesize, &pfferr);
	std::string childindex = itemindex + "," + std::to_string(i);
	std::string childnamestr = "";
	for(int j=0; j < childnamesize - 1; j++)
	    childnamestr += (char)childname[j];
	foldermap.insert(std::make_pair(childnamestr, childindex));
        FXTreeItem* childitem = new FXTreeItem(FXString(reinterpret_cast<char*>(childname)));
        treelist->appendItem(subitem, childitem);
	int childsubcnt = 0;
	reterr =libpff_folder_get_number_of_sub_folders(childfolder, &childsubcnt, &pfferr);
	if(childsubcnt > 0)
	    PopulateSubFolders(mailboxpath, childfolder, childitem, childindex);
	reterr = libpff_item_free(&childfolder, &pfferr);
    }
}

long WombatMail::SetTag(FXObject* sender, FXSelector, void*)
{
    FXString tagstr = ((FXMenuCommand*)sender)->getText();
    tablelist->setItemText(tablelist->getCurrentRow(), 1, tagstr);
    FXTreeItem* curitem = treelist->getCurrentItem();
    FXString rootstring = "";
    FXString mailboxpath = "";
    GetRootString(curitem, &rootstring);
    int found1 = rootstring.find(" (");
    int found2 = rootstring.find(")");
    mailboxpath = rootstring.mid(found1 + 2, found2 - found1 - 2) + rootstring.mid(0, found1);
    FXString idkeyvalue = mailboxpath + "\t" + curitem->getText() + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 2) + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 3) + "\t" + tablelist->getItemText(tablelist->getCurrentRow(), 4);
    for(int i=0; i < taggedlist.no(); i++)
    {
        if(taggedlist.at(i).contains(idkeyvalue))
            taggedlist.erase(i);
    }
    taggedlist.append(tagstr + "|" + idkeyvalue + "|" + plaintext->getText());

    return 1;
}

int main(int argc, char* argv[])
{
    FXApp* wr = new FXApp("Mail", "Wombat");

    wr->init(argc, argv);

    new WombatMail(wr);

    wr->create();
    wr->run();

    return 0;
}
