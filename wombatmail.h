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
#include "/usr/local/include/fox-1.7/fx.h"
#include "libpff.h"
#include "icons.h"
#include "managetags.h"
#include "aboutbox.h"
#include "viewer.h"

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
	FXIcon* openicon;
        FXButton* openbutton;
	FXIcon* managetagsicon;
	FXButton* managetagsbutton;
	FXIcon* previewicon;
	FXButton* previewbutton;
	FXIcon* publishicon;
	FXButton* publishbutton;
	FXIcon* abouticon;
	FXButton* aboutbutton;
        FXStatusBar* statusbar;
        FXFont* plainfont;
        std::string prevhivepath;
        std::string hivefilepath;
        std::string cmdhivepath = "";
        std::vector<std::filesystem::path> hives;
        std::vector<std::string> tags;
        FXArray<FXString> taggedlist;
        std::ifstream* filebufptr;
        Viewer* viewer;

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
            ID_LAST
        };
        WombatMail(FXApp* a);
        long OpenHive(FXObject*, FXSelector, void*);
        long OpenTagManager(FXObject*, FXSelector, void*);
	long OpenAboutBox(FXObject*, FXSelector, void*);
        long KeySelected(FXObject*, FXSelector, void*);
	long ValueSelected(FXObject*, FXSelector, void*);
        long TagMenu(FXObject*, FXSelector, void*);
        long SetTag(FXObject* sender, FXSelector, void*);
        long CreateNewTag(FXObject*, FXSelector, void*);
        long RemoveTag(FXObject*, FXSelector, void*);
        long PreviewReport(FXObject*, FXSelector, void*);
        long PublishReport(FXObject*, FXSelector, void*);
        long TableUp(FXObject*, FXSelector, void*);
	//void PopulateChildKeys(libregf_key_t* curkey, FXTreeItem* curitem, libregf_error_t* regerr);
	void GetRootString(FXTreeItem* curitem, FXString* rootstring);
	FXString ConvertWindowsTimeToUnixTimeUTC(uint64_t input);
        FXString ConvertUnixTimeToString(uint32_t input);
        FXString DecryptRot13(FXString encstr);
        FXchar Rot13Char(FXchar curchar);
        void AlignColumn(FXTable* curtable, int row, FXuint justify);
	void StatusUpdate(FXString tmptext)
	{
	    statusbar->getStatusLine()->setNormalText(tmptext);
	};
        virtual void create();

};

FXDEFMAP(WombatMail) WombatMailMap[]={
    FXMAPFUNC(SEL_CLICKED, WombatMail::ID_TREESELECT, WombatMail::KeySelected),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_OPEN, WombatMail::OpenHive),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_MANAGETAGS, WombatMail::OpenTagManager),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_ABOUT, WombatMail::OpenAboutBox),
    FXMAPFUNC(SEL_SELECTED, WombatMail::ID_TABLESELECT, WombatMail::ValueSelected),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE, WombatMail::ID_TABLESELECT, WombatMail::TagMenu),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_NEWTAG, WombatMail::CreateNewTag),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_SETTAG, WombatMail::SetTag),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_REMTAG, WombatMail::RemoveTag),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_PREVIEW, WombatMail::PreviewReport),
    FXMAPFUNC(SEL_COMMAND, WombatMail::ID_PUBLISH, WombatMail::PublishReport),
    FXMAPFUNC(SEL_KEYPRESS, WombatMail::ID_TABLESELECT, WombatMail::TableUp),
};

#endif // WOMBATMAIL_H
