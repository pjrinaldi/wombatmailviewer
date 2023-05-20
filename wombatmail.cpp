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
    //new FXSeparator(toolbar);
    managetagsicon = new FXPNGIcon(this->getApp(), managetags);
    managetagsbutton = new FXButton(toolbar, "", managetagsicon, this, ID_MANAGETAGS, BUTTON_TOOLBAR|FRAME_RAISED, 0,0,0,0, 4,4,4,4);
    managetagsbutton->setTipText("Manage Tags");
    headericon = new FXPNGIcon(this->getApp(), mail);
    headerbutton = new FXButton(toolbar, "", headericon, this, ID_HEADER, BUTTON_TOOLBAR|FRAME_RAISED, 0,0,0,0, 4,4,4,4);
    headerbutton->setTipText("Show Header");
    headerbutton->hide();
    //new FXSeparator(toolbar);
    previewicon = new FXPNGIcon(this->getApp(), reportpreview1);
    previewbutton = new FXButton(toolbar, "", previewicon, this, ID_PREVIEW, BUTTON_TOOLBAR|FRAME_RAISED, 0,0,0,0, 4,4,4,4);
    previewbutton->setTipText("Preview Report");
    publishicon = new FXPNGIcon(this->getApp(), paperairplane2);
    publishbutton = new FXButton(toolbar, "", publishicon, this, ID_PUBLISH, BUTTON_TOOLBAR|FRAME_RAISED, 0,0,0,0, 4,4,4,4);
    publishbutton->setTipText("Publish Report");
    //new FXSeparator(toolbar);
    abouticon = new FXPNGIcon(this->getApp(), helpcontents);
    aboutbutton = new FXButton(toolbar, "", abouticon, this, ID_ABOUT, BUTTON_TOOLBAR|FRAME_RAISED, 0,0,0,0, 4,4,4,4);
    aboutbutton->setTipText("About Wombat Mail Forensics");
    statusbar->getStatusLine()->setNormalText("Open a Mail Item to Begin");
    mailboxes.clear();
    tags.clear();
    taggedlist.clear();
}

void WombatMail::create()
{
    FXMainWindow::create();
    show(PLACEMENT_SCREEN);
    if(this->getApp()->getArgc() == 2)
    {
        cmdmailboxpath = std::string(this->getApp()->getArgv()[1]);
        int ret = OpenMailBox(NULL, 0, NULL);
        //int ret = OpenHive(NULL, 0, NULL);
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
	    tablelist->fitColumnsToContents(0);
    }
    // this should be the full file path the message is from\nMail folder\tDateTime\tFrom\tSubject\n\n;
    FXString idkeyvalue = "what to put here";
    //FXString idkeyvalue = statusbar->getStatusLine()->getText() + "\\" + tablelist->getItemText(tablelist->getCurrentRow(), 1);
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
    FXString idkeyvalue = "what to put here";
    //FXString idkeyvalue = statusbar->getStatusLine()->getText() + "\\" + tablelist->getItemText(tablelist->getCurrentRow(), 1);
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
    //std::cout << "mailboxpath: " << mailboxpath.text() << " curitemtext: " << curitemtext.text() << std::endl;
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
                //tablelist->setItemText(i, 1, "tag str");
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
	if(!tagstr.empty() && tagstr.length() > 5)
	    tablelist->fitColumnsToContents(1);
	*/
	/*
	if(namemax > 12)
	    tablelist->fitColumnsToContents(1);
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
    //std::cout << "root string: " << rootstring.text() << std::endl;
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
    else
    {
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
	//std::cout << "root string: " << rootstring.text() << std::endl;
	int found1 = rootstring.find(" (");
	int found2 = rootstring.find(")");
	mailboxpath = rootstring.mid(found1 + 2, found2 - found1 - 2) + rootstring.mid(0, found1);
	uint8_t mailboxtype = MailBoxType(mailboxpath.text());
	if(mailboxtype == 0x01) // PST/OST
	    PopulatePstEmail(mailboxpath, curitem->getText());
	else if(mailboxtype == 0x02) // MBOX
	    PopulateMboxEmail();
    }
    
    return 1;
}

void WombatMail::PopulatePstEmail(FXString mailboxpath, FXString curitemtext)
{
    int msgid = tablelist->getItemText(tablelist->getCurrentRow(), 0).toInt() - 1;
    //std::cout << "msgid: " << msgid << std::endl;
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
	/*
	// POPULATE ATTACHMENT PORTIONS
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
	    //qDebug() << "Attachment Name:" << QString::fromUtf8(reinterpret_cast<char*>(attachname));
	    size64_t attachsize = 0;
	    reterr = libpff_attachment_get_data_size(curattach, &attachsize, &pfferr);
	    QString attachstr = QString::fromUtf8(reinterpret_cast<char*>(attachname)) + " (" + QString::number(attachsize) + " bytes)";
	    //tmpitem->setText(attachstr);
	    ui->listwidget->addItem(new QListWidgetItem(attachstr));
	    //uint8_t attachbuf[attachsize];
	    //ssize_t bufread = 0;
	    //bufread = libpff_attachment_data_read_buffer(curattach, attachbuf, attachsize, &pfferr);
	}
	*/
	reterr = libpff_item_free(&curmsg, &pfferr);
	reterr = libpff_item_free(&selectedfolder, &pfferr);
	reterr = libpff_item_free(&rootfolder, &pfferr);
	reterr = libpff_file_close(pffile, &pfferr);
	reterr = libpff_file_free(&pffile, &pfferr);
    }
    libpff_error_free(&pfferr);
}

void WombatMail::PopulateMboxEmail(void)
{
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
        FXString curidkeyval = "not sure what to put here";
        //FXString curidkeyval = statusbar->getStatusLine()->getText() + "\\" + tablelist->getItemText(i, 1);
        FXString curtag = tablelist->getItemText(i, 0);
        for(int j=0; j < taggedlist.no(); j++)
        {
            std::size_t found = taggedlist.at(j).find("|");
            std::size_t rfound = taggedlist.at(j).rfind("|");
            FXString itemtag = taggedlist.at(j).mid(0, found);
            FXString itemhdr = taggedlist.at(j).mid(found+1, rfound - found - 1);
            if(FXString::compare(curidkeyval, itemhdr) == 0)
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
                    FXString itemcon = taggedlist.at(j).mid(rfound+1, taggedlist.at(j).length() - rfound);
                    if(itemtag == tags.at(i).c_str())
                    {
                        buf += "<div style='border-bottom: 1px solid black;'>\n";
                        buf += "<div>Key:&nbsp;&nbsp;&nbsp;&nbsp;" + itemhdr + "</div>\n";
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
        filename = FXString(cmdmailboxpath.c_str());
    if(!filename.empty())
    {
	std::string mailboxpath = filename.text();
        oldmailboxpath = mailboxpath;
	// check mailbox type
        uint8_t mailboxtype = 0x00;
	mailboxtype = MailBoxType(mailboxpath);
        //std::cout << "mail box type: " << (uint)mailboxtype << std::endl;
	if(mailboxtype == 0x01 || mailboxtype == 0x02 || mailboxtype == 0x03)
	{
            mailboxes.push_back(std::filesystem::canonical(mailboxpath));
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
                PopulateMbox(mailboxpath);
            }
            else if(mailboxtype == 0x03) // MSG
            {
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
    uint8_t mailboxtype = 0x00;
    libpff_error_t* pfferr = NULL;
    libolecf_error_t* olecerr = NULL;
    if(libpff_check_file_signature(mailboxpath.c_str(), &pfferr)) // is pst/ost
	mailboxtype = 0x01; // PST/OST
    else if(libolecf_check_file_signature(mailboxpath.c_str(), &olecerr)) // is msg
    {
        mailboxtype = 0x03; // MSG
    }
    else // might be mbox
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
    }
    libolecf_error_free(&olecerr);
    libpff_error_free(&pfferr);

    return mailboxtype;
}

void WombatMail::PopulateMsg(std::string mailboxpath)
{
}

void WombatMail::PopulateMbox(std::string mailboxpath)
{
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
            //std::string itempath = std::to_string(i) + ",";
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
	    //std::cout << "sub name index: " << i << " sub name string: " << subnamestr << std::endl;
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
    //std::cout << "parent item index: " << itemindex << std::endl;
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
	//std::cout << "childindex: " << childindex << " childnamestr: " << childnamestr << std::endl;
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
    tablelist->setItemText(tablelist->getCurrentRow(), 0, tagstr);
    FXString idkeyvalue = statusbar->getStatusLine()->getText() + "\\" + tablelist->getItemText(tablelist->getCurrentRow(), 1);
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
