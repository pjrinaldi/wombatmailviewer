#ifndef TAGMANAGER_H
#define TAGMANAGAER_H

// Copyright 2013-2019 Pasquale J. Rinaldi, Jr.
// Distrubted under the terms of the GNU General Public License version 2

#include <QCloseEvent>
#include <QStringList>
#include <QInputDialog>
#include "ui_tagmanager.h"

namespace Ui
{
    class TagManager;
}

class TagManager : public QDialog
{
    Q_OBJECT

public:
    TagManager(QWidget* parent = 0);
    ~TagManager();
    void SetTagList(QStringList* tagslist);

private slots:
    void HideClicked();
    void ModifyTag();
    void RemoveTag();
    void AddTag();
    void UpdateList();
    void SelectionChanged();
signals:
    void HideManagerWindow();
protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::TagManager* ui;

    QStringList* tags = NULL;
};

Q_DECLARE_METATYPE(TagManager*);

#endif // TAGMANAGER_H
