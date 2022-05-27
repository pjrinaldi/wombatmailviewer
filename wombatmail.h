#ifndef WOMBATMAIL_H
#define WOMBATMAIL_H

// Copyright 2022-2022 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QtEndian>
#include <QDateTime>
#include <QTimeZone>
#include <QDirIterator>
#include "ui_wombatmail.h"
#include "tagmanager.h"
#include "htmlviewer.h"
#include "aboutbox.h"
#include "libpff.h"
#include "cssstrings.h"

#include <mimetic/mimetic.h>
#include <vmime.hpp>
//#include <KF5/KMime>
//#include <KF5/KMime/kmime/kmime_message.h>

#define TICKS_PER_SECOND 10000000
#define EPOCH_DIFFERENCE 11644473600LL
#define NSEC_BTWN_1904_1970	(uint32_t) 2082844800U

namespace Ui
{
    class WombatMail;
}

class WombatMail : public QMainWindow
{
    Q_OBJECT

public:
    explicit WombatMail(QWidget* parent = 0);
    ~WombatMail();
    //void LoadRegistryFile(void);
    //void LoadMBoxFile(QString mboxid, QString mboxname);

private slots:
    void OpenMailBox(void);
    void ManageTags(void);
    void PreviewReport(void);
    void PublishReport(void);
    void ShowAbout(void);
    void MailBoxSelected(void);
    void MailItemSelected(void);
    //void KeySelected(void);
    //void ValueSelected(void);
    //void EmailSelected();
    void TagMenu(const QPoint &point);
    void SetTag(void);
    void CreateNewTag(void);
    void RemoveTag(void);
    void UpdateTagsMenu(void);
    void UpdatePreviewLinks(void);
    void StatusUpdate(QString tmptext)
    {
        statuslabel->setText(tmptext);
    };
protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::WombatMail* ui;
    QLabel* statuslabel;
    QString mboxfilepath;
    QString prevmboxpath;
    QFile mboxfile;
    uint8_t mailboxtype;
    //void PopulateChildKeys(libregf_key_t* curkey, QTreeWidgetItem* curitem, libregf_error_t* curerr);
    QString DecryptRot13(QString encstr);
    QChar Rot13Char(QChar curchar);
    uint8_t MailBoxType(QString mailboxpath);
    void PopulateMbox(QString mailboxpath);
    void PopulateMboxEmail(void);
    QString ConvertUnixTimeToString(uint32_t input);
    QString ConvertWindowsTimeToUnixTimeUTC(uint64_t input);
    int GetRootIndex(QTreeWidgetItem* curitem);
    QMenu* tagmenu;
    QAction* newtagaction;
    QAction* remtagaction;
    QString htmlentry;
    QString htmlvalue;
    QString reportstring;
    QString prehtml;
    QString psthtml;
    QByteArray reporttimezone;
    QStringList tags;
    QStringList taggeditems;
    QStringList mboxes;
    QRegExp mboxheader;

    vmime::charset ch;
};

#endif // WOMBATMAIL_H
