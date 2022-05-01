#include "tagmanager.h"

// Copyright 2013-2020 Pasquale J. Rinaldi, Jr.
// Distrubted under the terms of the GNU General Public License version 2

TagManager::TagManager(QWidget* parent) : QDialog(parent), ui(new Ui::TagManager)
{
    ui->setupUi(this);
    connect(ui->newbutton, SIGNAL(clicked()), this, SLOT(AddTag()));
    connect(ui->modifybutton, SIGNAL(clicked()), this, SLOT(ModifyTag()));
    connect(ui->removebutton, SIGNAL(clicked()), this, SLOT(RemoveTag()));
    connect(ui->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(SelectionChanged()));
    ui->removebutton->setEnabled(false);
    ui->modifybutton->setEnabled(false);
}

TagManager::~TagManager()
{
    delete ui;
}

void TagManager::SetTagList(QStringList* tagslist)
{
    tags = tagslist;
    if(tags != NULL)
	UpdateList();
}

void TagManager::HideClicked()
{
    emit HideManagerWindow();
    this->close();
}

void TagManager::ModifyTag()
{
    QString selectedtag = ui->listWidget->currentItem()->text();
    int tagindex = tags->indexOf(selectedtag);
    QString tmpstr = "";
    QString modtagname = "";
    QInputDialog* modtagdialog = new QInputDialog(this);
    modtagdialog->setCancelButtonText("Cancel");
    modtagdialog->setInputMode(QInputDialog::TextInput);
    modtagdialog->setLabelText("Modify Tag Name");
    modtagdialog->setOkButtonText("Modify");
    modtagdialog->setTextEchoMode(QLineEdit::Normal);
    modtagdialog->setWindowTitle("Modify Tag");
    modtagdialog->setTextValue(selectedtag);
    if(modtagdialog->exec())
        modtagname = modtagdialog->textValue();
    if(!modtagname.isEmpty())
    {
	ui->listWidget->currentItem()->setText(modtagname);
	tags->replace(tagindex, modtagname);
    }
}

void TagManager::RemoveTag()
{
    QString selectedtag = ui->listWidget->currentItem()->text();
    int tagindex = tags->indexOf(selectedtag);
    tags->removeAt(tagindex);
    ui->listWidget->removeItemWidget(ui->listWidget->currentItem());
    ui->removebutton->setEnabled(false);
    ui->modifybutton->setEnabled(false);
    UpdateList();
}

void TagManager::AddTag()
{
    QString tagname = "";
    QInputDialog* newtagdialog = new QInputDialog(this);
    newtagdialog->setCancelButtonText("Cancel");
    newtagdialog->setInputMode(QInputDialog::TextInput);
    newtagdialog->setLabelText("Enter Tag Name");
    newtagdialog->setOkButtonText("Create Tag");
    newtagdialog->setTextEchoMode(QLineEdit::Normal);
    newtagdialog->setWindowTitle("New Tag");
    if(newtagdialog->exec())
        tagname = newtagdialog->textValue();
    if(!tagname.isEmpty())
    {
	tags->append(tagname);
	UpdateList();
    }
}

void TagManager::SelectionChanged()
{
    ui->removebutton->setEnabled(true);
    ui->modifybutton->setEnabled(true);
}

void TagManager::UpdateList()
{
    ui->listWidget->clear();
    for(int i=0; i < tags->count(); i++)
    {
	new QListWidgetItem(tags->at(i), ui->listWidget);
    }
}

void TagManager::closeEvent(QCloseEvent* e)
{
    emit HideManagerWindow();
    e->accept();
}
