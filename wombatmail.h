#ifndef WOMBATMAIL_H
#define WOMBATMAIL_H

// Copyright 2013-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <filesystem>
#include <byteswap.h>
#include <time.h>
#include <regex>
#include <sys/stat.h>

#include "fox-1.7/fx.h"
#include "icons.h"

#include "libpff.h"
#include "vmime/vmime.hpp"

#include "managetags.h"
#include "aboutbox.h"
#include "viewer.h"
#include "msg.h"

#define TICKS_PER_SECOND 10000000
#define EPOCH_DIFFERENCE 11644473600LL
#define NSEC_BTWN_1904_1970	(uint32_t) 2082844800U

class WombatMail : public FXMainWindow
{
    FXDECLARE(WombatMail)

    private:
        FXVerticalFrame* mainframe;
        FXSplitter* hsplitter;
        FXSplitter* vsplitter;
        FXTreeList* treelist;
        FXToolBar* toolbar;
        FXText* plaintext;
        FXTable* tablelist;
        FXTreeItem* rootitem;
	FXList* attachmentlist;
	FXIcon* openicon;
        FXButton* openbutton;
	FXIcon* managetagsicon;
	FXButton* managetagsbutton;
        FXIcon* headericon;
        FXButton* headerbutton;
	FXIcon* previewicon;
	FXButton* previewbutton;
	FXIcon* publishicon;
	FXButton* publishbutton;
	FXIcon* abouticon;
	FXButton* aboutbutton;
        FXStatusBar* statusbar;
        FXFont* plainfont;
        std::string oldmailboxpath;
        std::string cmdmailboxpath = "";
        std::vector<std::string> tags;
        FXArray<FXString> taggedlist;
        std::ifstream* filebufptr;
        Viewer* viewer;
        std::map<std::string, std::string> foldermap;
        std::vector<std::string> msgs;
        vmime::charset ch;
        std::vector<AttachmentInfo> msgattachments;
        OutlookMessage* msg;

    protected:
        WombatMail() {}

    public:
        enum
        {
            ID_TREELIST = 1,
            ID_OPEN = 100,
            ID_TREESELECT = 101,
	    ID_MANAGETAGS = 102,
	    ID_PREVIEW = 103,
	    ID_PUBLISH = 104,
	    ID_ABOUT = 105,
	    ID_TABLESELECT = 106,
            ID_TAGMENU = 107,
            ID_NEWTAG = 108,
            ID_SETTAG = 109,
            ID_REMTAG = 110,
            ID_MOVE_UP = 111,
            ID_HEADER = 112,
	    ID_LISTSELECT = 113,
            ID_LAST
        };
        WombatMail(FXApp* a);
        long OpenMailBox(FXObject*, FXSelector, void*);
        long OpenTagManager(FXObject*, FXSelector, void*);
	long OpenAboutBox(FXObject*, FXSelector, void*);
        long MailBoxSelected(FXObject*, FXSelector, void*);
	long MessageSelected(FXObject*, FXSelector, void*);
        long TagMenu(FXObject*, FXSelector, void* ptr);
        long SetTag(FXObject* sender, FXSelector, void*);
        long CreateNewTag(FXObject*, FXSelector, void*);
        long RemoveTag(FXObject*, FXSelector, void*);
        long PreviewReport(FXObject*, FXSelector, void*);
        long PublishReport(FXObject*, FXSelector, void*);
        long TableUp(FXObject*, FXSelector, void*);
        long ShowHeader(FXObject*, FXSelector, void* checkstate);
        long AttachmentSelected(FXObject*, FXSelector, void*);
        
        uint8_t MailBoxType(std::string mailboxpath);
        void PopulateMbox(std::string mailboxpath);
        void PopulatePst(std::string mailboxpath);
        void PopulateMsg(std::string mailboxpath);
        void PopulateSubFolders(std::string mailboxpath, libpff_item_t* subfolder, FXTreeItem* subitem, std::string subindex);
        void PopulatePstFolder(FXString mailboxpath, FXString curitemtext);
	void PopulatePstEmail(FXString mailboxpath, FXString curitemtext);
	void PopulateMboxEmail(FXString mailboxpath, FXString curitemtext);
	void PopulateEml(FXString mailboxpath);
	void GetRootString(FXTreeItem* curitem, FXString* rootstring);
	FXString ConvertWindowsTimeToUnixTimeUTC(uint64_t input);
        FXString ConvertUnixTimeToString(uint32_t input);
        void AlignColumn(FXTable* curtable, int row, FXuint justify);
	void StatusUpdate(FXString tmptext)
	{
	    statusbar->getStatusLine()->setNormalText(tmptext);
	};
        virtual void create();
        void GetMimeSubject(std::string* msg, std::string* subject);
        void GetMimeFrom(std::string* msg, std::string* from);
        void GetMimeDate(std::string* msg, std::string* date);
        void GetMessageContent(std::string* msg, std::string* content);
	void GetMimeAttachments(std::string* msg);
};

FXDEFMAP(WombatMail) WombatMailMap[]={
    FXMAPFUNC(SEL_CLICKED, WombatMail::ID_TREESELECT, WombatMail::MailBoxSelected),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_OPEN, WombatMail::OpenMailBox),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_MANAGETAGS, WombatMail::OpenTagManager),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_HEADER, WombatMail::ShowHeader),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_ABOUT, WombatMail::OpenAboutBox),
    FXMAPFUNC(SEL_SELECTED, WombatMail::ID_TABLESELECT, WombatMail::MessageSelected),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE, WombatMail::ID_TABLESELECT, WombatMail::TagMenu),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_NEWTAG, WombatMail::CreateNewTag),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_SETTAG, WombatMail::SetTag),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_REMTAG, WombatMail::RemoveTag),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_PREVIEW, WombatMail::PreviewReport),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_PUBLISH, WombatMail::PublishReport),
    FXMAPFUNC(SEL_KEYPRESS, WombatMail::ID_TABLESELECT, WombatMail::TableUp),
    FXMAPFUNC(SEL_SELECTED, WombatMail::ID_LISTSELECT, WombatMail::AttachmentSelected),
};

#endif // WOMBATMAIL_H
