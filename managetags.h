#ifndef MANAGETAGS_H
#define MANAGETAGS_H

// Copyright 2013-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "icons.h"

#include "fox-1.7/fx.h"

class ManageTags : public FXDialogBox
{
    FXDECLARE(ManageTags)

    private:
        FXVerticalFrame* mainframe;
        FXHorizontalFrame* buttonframe;
        FXLabel* mainlabel;
        FXList* taglist;
        FXIcon* newicon;
        FXIcon* editicon;
        FXIcon* remicon;
        FXButton* newbutton;
        FXButton* editbutton;
        FXButton* rembutton;

        std::vector<std::string>* tags = NULL;
        FXArray<FXString>* tlist = NULL;

    protected:
        ManageTags() {}

    public:
        enum
        {
            ID_TREELIST = 1,
            ID_LISTSELECT = 100,
            ID_NEWTAG = 101,
            ID_EDITTAG = 102,
            ID_REMTAG = 103,
            ID_LAST
        };
        ManageTags(FXWindow* parent, const FXString& title);
        void SetTagList(std::vector<std::string>* tagslist, FXArray<FXString>* taggedlist);
        void UpdateList();
        void UpdateTaggedList(FXString curtag, FXString modtag);
        long AddTag(FXObject*, FXSelector, void*);
        long ModifyTag(FXObject*, FXSelector, void*);
        long RemoveTag(FXObject*, FXSelector, void*);
        long ListSelection(FXObject*, FXSelector, void*);

};

FXDEFMAP(ManageTags) ManageTagsMap[]={
    FXMAPFUNC(SEL_COMMAND, ManageTags::ID_NEWTAG, ManageTags::AddTag),
    FXMAPFUNC(SEL_COMMAND, ManageTags::ID_EDITTAG, ManageTags::ModifyTag),
    FXMAPFUNC(SEL_COMMAND, ManageTags::ID_REMTAG, ManageTags::RemoveTag),
    FXMAPFUNC(SEL_CLICKED, ManageTags::ID_LISTSELECT, ManageTags::ListSelection),
};

#endif // MANAGETAGS_H
