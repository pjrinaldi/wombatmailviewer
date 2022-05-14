#include "wombatmail.h"

// Copyright 2013-2020 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

WombatMail::WombatMail(QWidget* parent) : QMainWindow(parent), ui(new Ui::WombatMail)
{
    ui->setupUi(this);
    this->menuBar()->hide();
    statuslabel = new QLabel(this);
    this->statusBar()->addPermanentWidget(statuslabel, 0);
    StatusUpdate("Open a Mail Box to Begin");
    ui->tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tablewidget->setHorizontalHeaderLabels({"ID", "Tag", "From", "Date Time", "Subject"});
    connect(ui->treewidget, SIGNAL(itemSelectionChanged()), this, SLOT(MailBoxSelected()), Qt::DirectConnection);
    connect(ui->tablewidget, SIGNAL(itemSelectionChanged()), this, SLOT(MailItemSelected()), Qt::DirectConnection);
    connect(ui->actionOpenMailBox, SIGNAL(triggered()), this, SLOT(OpenMailBox()), Qt::DirectConnection);
    connect(ui->actionManageTags, SIGNAL(triggered()), this, SLOT(ManageTags()), Qt::DirectConnection);
    connect(ui->actionPreviewReport, SIGNAL(triggered()), this, SLOT(PreviewReport()), Qt::DirectConnection);
    connect(ui->actionPublish, SIGNAL(triggered()), this, SLOT(PublishReport()), Qt::DirectConnection);
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(ShowAbout()), Qt::DirectConnection);
    // initialize temp directory for html code...
    QDir tmpdir;
    tmpdir.mkpath(QDir::tempPath() + "/wm");
    tmpdir.mkpath(QDir::tempPath() + "/wm/tagged");
    // initialize Preview Report HTML code
    prehtml = "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'></head><body style='" + ReturnCssString(0) + "'>\n";
    prehtml += "<div style='" + ReturnCssString(1) + "'><h1><span id='casename'>MailBox Report</span></h1></div>\n";
    psthtml = "</body></html>";

    tags.clear();
    tagmenu = new QMenu(ui->tablewidget);
    UpdateTagsMenu();

    mboxes.clear();

    ui->tablewidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tablewidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(TagMenu(const QPoint &)), Qt::DirectConnection);
    mboxheader = QRegExp(QStringLiteral("^From .*[0-9][0-9]:[0-9][0-9]")); // kmbox regexp expression
    ui->textbrowser->setOpenExternalLinks(false);
    ui->textbrowser->setOpenLinks(false);
}

WombatMail::~WombatMail()
{
    delete ui;
    QDir tmpdir(QDir::tempPath() + "/wr");
    tmpdir.removeRecursively();
}

uint8_t WombatMail::MailBoxType(QString mailboxpath)
{
    int reterr = 0;
    uint8_t mailboxtype = 0x00;
    libpff_error_t* pfferr = NULL;
    if(libpff_check_file_signature(mailboxpath.toStdString().c_str(), &pfferr)) // is pst/ost
	mailboxtype = 0x01; // PST/OST
    else // might be mbox
    {
	QFile mboxfile(mailboxpath);
	if(!mboxfile.isOpen())
	    mboxfile.open(QIODevice::ReadOnly | QIODevice::Text);
	if(mboxfile.isOpen())
	{
	    while(!mboxfile.atEnd())
	    {
		QString line = mboxfile.readLine();
		int pos = mboxheader.indexIn(line);
		if(pos >= 0)
		{
		    mailboxtype = 0x02; // mbox
		    break;
		}
	    }
	    mboxfile.close();
	}
    }
    libpff_error_free(&pfferr);

    return mailboxtype;
}

void WombatMail::PopulateMbox(QString mfpath)
{
    QString layout = "";
    QList<qint64> poslist;
    QList<qint64> linelength;
    poslist.clear();
    linelength.clear();
    mboxfile.setFileName(mfpath);
    if(!mboxfile.isOpen())
        mboxfile.open(QIODevice::ReadOnly | QIODevice::Text);
    // split mbox into messages...
    if(mboxfile.isOpen())
    {
        while(!mboxfile.atEnd())
        {
            QString line = mboxfile.readLine();
            int pos = mboxheader.indexIn(line);
            if(pos >= 0)
            {
                poslist.append(mboxfile.pos());
                linelength.append(line.count());
            }
        }
        poslist.append(mboxfile.size());
        mboxfile.close();
    }
    //qDebug() << "poslist:" << poslist;
    //qDebug() << "linelength:" << linelength;
    ui->tablewidget->clear();
    ui->tablewidget->setHorizontalHeaderLabels({"ID", "Tag", "From", "Date Time", "Subject"});
    ui->plaintext->setPlainText("");
    ui->tablewidget->setRowCount(poslist.count() - 1);
    QStringList headers;
    QStringList bodies;
    QStringList msgs;
    headers.clear();
    bodies.clear();
    msgs.clear();
    for(int i=0; i < poslist.count() - 1; i++)
    {
        int splitpos = 0;
        if(!mboxfile.isOpen())
            mboxfile.open(QIODevice::ReadOnly | QIODevice::Text);
        int pos = poslist.at(i);
        //qDebug() << "pos:" << pos << "endpos:" << poslist.at(i+1) - poslist.at(i) - linelength.at(i);
        mboxfile.seek(poslist.at(i));
        while(pos <= poslist.at(i+1) - linelength.at(i))
        {
            QString line = mboxfile.readLine();
            if(line == "\n")
            {
                splitpos = mboxfile.pos() - poslist.at(i);
                //qDebug() << "splitpos1:" << splitpos;
                break;
            }
            pos = mboxfile.pos();
        }
        mboxfile.seek(poslist.at(i));
        headers.append(mboxfile.read(splitpos));
        bodies.append(mboxfile.read(poslist.at(i+1) - linelength.at(i) - splitpos - poslist.at(i)));
        mboxfile.seek(poslist.at(i));
        msgs.append(mboxfile.read(poslist.at(i+1) - linelength.at(i) - poslist.at(i)));
        
        mboxfile.close();
    }
    for(int i=0; i < headers.count(); i++)
    {
	QStringList mailheaders = { "From: ", "Date: ", "Bcc: ", "To: ", "Sender: ", "Message-ID: ", "Subject: ", "cc: ", "Comment: ", "In-Reply-To: ", "X-Special-action: ", "References: ", "Newsgroups: ", "Lines: ", "Message-Id: ", "MIME-Version: ", "Content-Type: ", "Content-Transfer-Encoding: ", "Mime-Version: " };
        QList<qint64> headeritems;
        headeritems.clear();
        int fromstart = msgs.at(i).indexOf("From: ");
        int subjstart = msgs.at(i).indexOf("Subject: ");
        int datestart = msgs.at(i).indexOf("Date: ");
	// get offset for the mail headers
	for(int j=0; j < mailheaders.count(); j++)
	    headeritems.append(msgs.at(i).indexOf(mailheaders.at(j), Qt::CaseInsensitive));
	// get all the offsets for the line returns \n
	int off = 0;
	while(off <= msgs.at(i).length())
	{
	    off = msgs.at(i).indexOf("\n", off+1);
	    if(off == -1)
		break;
	    headeritems.append(off);
	}
        std::sort(headeritems.begin(), headeritems.end());
        auto last = std::unique(headeritems.begin(), headeritems.end());
        headeritems.erase(last, headeritems.end());
        headeritems.removeFirst();
	headeritems.append(msgs.at(i).length());
        //qDebug() << "headeritems after unique:" << headeritems;

        QString datestr = "";
        QString subjstr = "";
        QString fromstr = msgs.at(i).mid(headeritems.at(headeritems.indexOf(fromstart)) + 6, headeritems.at(headeritems.indexOf(fromstart) + 1) - headeritems.at(headeritems.indexOf(fromstart)) - 6);
        if(datestart > -1)
            datestr = msgs.at(i).mid(headeritems.at(headeritems.indexOf(datestart)) + 6, headeritems.at(headeritems.indexOf(datestart) + 1) - headeritems.at(headeritems.indexOf(datestart)) - 6);
        if(subjstart > -1)
            subjstr = msgs.at(i).mid(headeritems.at(headeritems.indexOf(subjstart)) + 9, headeritems.at(headeritems.indexOf(subjstart) + 1) - headeritems.at(headeritems.indexOf(subjstart)) - 9);
        QTableWidgetItem* tmpitem = new QTableWidgetItem(QString::number(i+1));
        tmpitem->setToolTip(QString(QString::number(poslist.at(i)) + "," + QString::number(poslist.at(i+1) - poslist.at(i) - linelength.at(i))));
        ui->tablewidget->setItem(i, 0, tmpitem);
        ui->tablewidget->setItem(i, 2, new QTableWidgetItem(fromstr.remove("\n")));
        ui->tablewidget->setItem(i, 3, new QTableWidgetItem(datestr.remove("\n")));
        ui->tablewidget->setItem(i, 4, new QTableWidgetItem(subjstr.remove("\n")));
    }
}

void WombatMail::OpenMailBox()
{
    if(prevmboxpath.isEmpty())
	prevmboxpath = QDir::homePath();
    QFileDialog openmboxdialog(this, tr("Open Mail Box"), prevmboxpath);
    openmboxdialog.setLabelText(QFileDialog::Accept, "Open");
    if(openmboxdialog.exec())
    {
        mboxfilepath = openmboxdialog.selectedFiles().first();
	prevmboxpath = mboxfilepath;
	// check mailbox type
	mailboxtype = MailBoxType(mboxfilepath);
	if(mailboxtype == 0x01 || mailboxtype == 0x02)
	{
	    mboxes.append(mboxfilepath);
	    mboxfile.setFileName(mboxfilepath);
	    if(mboxfile.exists())
	    {
		QTreeWidgetItem* rootitem = new QTreeWidgetItem(ui->treewidget);
		rootitem->setText(0, mboxfilepath.split("/").last().toUpper() + " (" + mboxfilepath + ")");
		ui->treewidget->addTopLevelItem(rootitem);
		ui->treewidget->setCurrentItem(rootitem);
	    }
	    if(mailboxtype == 0x01) // PST/OST
	    {
		//populate tree and table
	    }
	    else if(mailboxtype == 0x02) // MBOX
	    {
                //PopulateMbox(mboxfilepath);
		//populate table which needs to be reproducible
	    }
	}
	else // OTHER FILE
	{
	    // not a supported type.
	}
	
	/*
        int reterr = 0;
        libpff_error_t* pfferr = NULL;
        if(libpff_check_file_signature(mboxfilepath.toStdString().c_str(), &pfferr))
        {
            qDebug() << "a pst/ost file, so open...";
            libpff_file_t* pfffile = NULL;
            reterr = libpff_file_initialize(&pfffile, &pfferr);
            reterr = libpff_file_open(pfffile, mboxfilepath.toStdString().c_str(), LIBPFF_OPEN_READ, &pfferr);
            libpff_file_close(pfffile, &pfferr);
            libpff_file_free(&pfffile, &pfferr);
        }
	*/
    }
}

void WombatMail::ManageTags()
{
    TagManager* tagmanager = new TagManager(this);
    tagmanager->SetTagList(&tags);
    tagmanager->exec();
    UpdateTagsMenu();
}

void WombatMail::UpdatePreviewLinks()
{
    // POSSIBLY REBUILD THE MAIN PAGE EVERY TIME, RATHER THAN FIND AND REPLACE...
    QDir tmpdir(QDir::tempPath() + "/wr/tagged");
    tmpdir.removeRecursively();
    tmpdir.mkpath(QDir::tempPath() + "/wr/tagged");
    QString curcontent = "";
    curcontent += "<div id='toc'><h2>Contents</h2>";
    for(int i=0; i < tags.count(); i++)
    {
        curcontent += "<span" + QString::number(i) + "'><a href='#t" + QString::number(i) + "'>" + tags.at(i) + "</a></span><br/>\n";
    }
    curcontent += "<h2>Tagged Items</h2>";
    for(int i=0; i < tags.count(); i++)
    {
        curcontent += "<div id='t" + QString::number(i) + "'><h3>" + tags.at(i) + "</h3><br/><table><tr>";
        for(int j=0; j < taggeditems.count(); j++)
        {
            if(taggeditems.at(j).split("|", Qt::SkipEmptyParts).at(0) == tags.at(i))
            {
                curcontent += "<td style='" + ReturnCssString(11) + "'><a href='" + QDir::tempPath() + "/wr/tagged/" + QString::number(i) + "-" + QString::number(j) + ".html'>" + taggeditems.at(j).split("|").at(1) + "</a></td>";
                QString htmlvalue = "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'></head><body style='" + ReturnCssString(0) + "'>";
                htmlvalue += "<div style='" + ReturnCssString(1) + "'>Registry Analysis</div><br/>";
                htmlvalue += "<pre>";
                htmlvalue += taggeditems.at(j).split("|").at(2).toUtf8();
                htmlvalue + "</pre><body></html>";
                QFile htmlfile(QDir::tempPath() + "/wr/tagged/" + QString::number(i) + "-" + QString::number(j) + ".html");
                if(!htmlfile.isOpen())
                    htmlfile.open(QIODevice::WriteOnly | QIODevice::Text);
                if(htmlfile.isOpen())
                {
                    htmlfile.write(htmlvalue.toStdString().c_str());
                    htmlfile.close();
                }
            }
        }
        curcontent += "</tr></table></div><br/>\n";
    }
    reportstring = prehtml + curcontent + psthtml;
    QFile indxfile(QDir::tempPath() + "/wr/index.html");
    if(!indxfile.isOpen())
        indxfile.open(QIODevice::WriteOnly | QIODevice::Text);
    if(indxfile.isOpen())
    {
        indxfile.write(reportstring.toStdString().c_str());
        indxfile.close();
    }
}

void WombatMail::PreviewReport()
{
    UpdatePreviewLinks();
    HtmlViewer* htmlviewer = new HtmlViewer();
    htmlviewer->LoadHtml(QDir::tempPath() + "/wr/index.html");
    htmlviewer->show();
}

void WombatMail::PublishReport()
{
    UpdatePreviewLinks();
    QString savepath = QFileDialog::getExistingDirectory(this, tr("Select Report Folder"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!savepath.isEmpty())
    {
        // Make tagged path to store tagged registry files
        QDir tmppath;
        tmppath.mkpath(savepath + "/tagged/");
        QFile::copy(QDir::tempPath() + "/wr/index.html", savepath + "/index.html");
        QDirIterator it(QString(QDir::tempPath() + "/wr/tagged"), QDirIterator::NoIteratorFlags);
        while(it.hasNext())
        {
            QString curfile = it.next();
            if(curfile.endsWith("html"))
            {
                QFile::copy(curfile, savepath + "/tagged/" + curfile.split("/").last());
            }
        }
    }
}

void WombatMail::ShowAbout()
{
    AboutBox* aboutbox = new AboutBox();
    aboutbox->exec();
}

void WombatMail::CreateNewTag()
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
	tags.append(tagname);
        UpdateTagsMenu();
    }
    ui->tablewidget->selectedItems().first()->setText(tagname);
    QString idkeyvalue = statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text();
    for(int i=0; i < taggeditems.count(); i++)
    {
        if(taggeditems.at(i).contains(idkeyvalue))
            taggeditems.removeAt(i);
    }
    taggeditems.append(tagname + "|" + statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text() + "|" + ui->plaintext->toPlainText());
}

void WombatMail::UpdateTagsMenu()
{
    tagmenu->clear();
    newtagaction = new QAction("New Tag", tagmenu);
    newtagaction->setIcon(QIcon(":/bar/newtag"));
    connect(newtagaction, SIGNAL(triggered()), this, SLOT(CreateNewTag()));
    tagmenu->addAction(newtagaction);
    tagmenu->addSeparator();
    for(int i=0; i < tags.count(); i++)
    {
	QAction* tmpaction = new QAction(tags.at(i), tagmenu);
	tmpaction->setIcon(QIcon(":/bar/tag"));
	tmpaction->setData(QVariant("t" + QString::number(i)));
	connect(tmpaction, SIGNAL(triggered()), this, SLOT(SetTag()));
	tagmenu->addAction(tmpaction);
    }
    tagmenu->addSeparator();
    remtagaction = new QAction("Remove Tag", tagmenu);
    remtagaction->setIcon(QIcon(":/bar/removetag"));
    connect(remtagaction, SIGNAL(triggered()), this, SLOT(RemoveTag()));
    tagmenu->addAction(remtagaction);
}

void WombatMail::SetTag()
{
    QAction* tagaction = qobject_cast<QAction*>(sender());
    QString idkeyvalue = statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text();
    if(!ui->tablewidget->selectedItems().first()->text().isEmpty())
    {
        for(int i=0; i < taggeditems.count(); i++)
        {
            if(taggeditems.at(i).contains(idkeyvalue))
                taggeditems.removeAt(i);
        }
    }
    taggeditems.append(tagaction->iconText() + "|" + statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text() + "|" + ui->plaintext->toPlainText());

    ui->tablewidget->selectedItems().first()->setText(tagaction->iconText());
}

void WombatMail::RemoveTag()
{
    ui->tablewidget->selectedItems().first()->setText("");
    QString idkeyvalue = statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text();
    for(int i=0; i < taggeditems.count(); i++)
    {
        if(taggeditems.at(i).contains(idkeyvalue))
            taggeditems.removeAt(i);
    }
}

void WombatMail::MailBoxSelected(void)
{
    //qDebug() << "populate mail items here...";
    if(mailboxtype == 0x01) // PST/OST
    {
	//populate tree and table
    }
    else if(mailboxtype == 0x02) // MBOX
    {
	PopulateMbox(mboxfilepath);
	//populate table which needs to be reproducible
    }
}

void WombatMail::MailItemSelected(void)
{
    if(mailboxtype == 0x01) // PST/OST
    {
	//populate tree and table
    }
    else if(mailboxtype == 0x02) // MBOX
    {
	PopulateMboxEmail();
	//populate table which needs to be reproducible
    }
    //qDebug() << "populate contents for selected mail item...";
}

void WombatMail::PopulateMboxEmail()
{
    mboxfilepath = ui->treewidget->currentItem()->text(0).split("(").last().split(")").first();
    QString layout = ui->tablewidget->item(ui->tablewidget->currentRow(), 0)->toolTip();
    mboxfile.setFileName(mboxfilepath);
    if(!mboxfile.isOpen())
	mboxfile.open(QIODevice::ReadOnly | QIODevice::Text);
    if(mboxfile.isOpen())
    {
	mboxfile.seek(layout.split(",").at(0).toULongLong());
	QString msg = mboxfile.read(layout.split(",").at(1).toULongLong());
	ui->plaintext->setPlainText(msg);
	ui->textbrowser->setHtml(msg);
	mboxfile.close();
    }
}
/*
void MBoxDialog::EmailSelected()
{
    QString mboxid = ui->tablewidget->item(ui->tablewidget->currentRow(), 0)->text().split("-m").at(0);
    QString layout = ui->tablewidget->item(ui->tablewidget->currentRow(), 0)->toolTip();
    QFile mboxfile(wombatvariable.tmpfilepath + mboxid + "-fhex");
    if(!mboxfile.isOpen())
        mboxfile.open(QIODevice::ReadOnly | QIODevice::Text);
    if(mboxfile.isOpen())
    {
        mboxfile.seek(layout.split(";").at(0).split(",").at(0).toULongLong());
        htmlvalue = mboxfile.read(layout.split(";").at(0).split(",").at(1).toULongLong());
        ui->mailcontent->setPlainText(htmlvalue);
        mboxfile.close();
    }
}
*/

/*
void MBoxDialog::LoadMBoxFile(QString mboxid, QString mboxname)
{
    mboxparentname = mboxname;
    ui->tablewidget->clear();
    ui->mailcontent->setPlainText("");
    QDir mailboxdir(wombatvariable.tmpmntpath + "mailboxes/");
    QStringList mailboxfiles = mailboxdir.entryList(QStringList() << QString(mboxid + "-m*.prop"), QDir::Files);
    ui->tablewidget->setHorizontalHeaderLabels({"ID", "From", "Date Time", "Subject", "Tag"});
    ui->tablewidget->setRowCount(mailboxfiles.count());
    for(int i=0; i < mailboxfiles.count(); i++)
    {
        QTableWidgetItem* tmpitem = new QTableWidgetItem(QString(mailboxfiles.at(i)).remove(".prop"));
        QFile propfile(wombatvariable.tmpmntpath + "mailboxes/" + mailboxfiles.at(i));
        if(!propfile.isOpen())
            propfile.open(QIODevice::ReadOnly | QIODevice::Text);
        if(propfile.isOpen())
        {
            while(!propfile.atEnd())
            {
                QString tmpstr = propfile.readLine();
                if(tmpstr.startsWith("From|"))
                    ui->tablewidget->setItem(i, 1, new QTableWidgetItem(tmpstr.split("|").at(1)));
                if(tmpstr.startsWith("Date|"))
                    ui->tablewidget->setItem(i, 2, new QTableWidgetItem(tmpstr.split("|").at(1)));
                if(tmpstr.startsWith("Subject|"))
                    ui->tablewidget->setItem(i, 3, new QTableWidgetItem(tmpstr.split("|").at(1)));
                if(tmpstr.startsWith("Layout|"))
                    tmpitem->setToolTip(tmpstr.split("|").at(1));
            }
            propfile.close();
        }
        ui->tablewidget->setItem(i, 0, tmpitem);
        QString tagstr = "";
        for(int j=0; j < mboxtaglist.count(); j++)
        {
            if(mboxtaglist.at(j).contains(QString(mailboxfiles.at(i)).remove(".prop")))
                tagstr = mboxtaglist.at(j).split("|", Qt::SkipEmptyParts).last();
        }
        ui->tablewidget->setItem(i, 4, new QTableWidgetItem(tagstr));
    }
    this->show();
}
*/

/*
void WombatMail::ValueSelected(void)
{
    if(ui->tablewidget->selectedItems().count() > 0)
    {
	QTreeWidgetItem* curitem = ui->treewidget->selectedItems().first();
	int rootindex = GetRootIndex(curitem);
	mboxfilepath = mboxes.at(rootindex);
	int valueindex = ui->tablewidget->selectedItems().at(1)->row();
	QString keypath = statuslabel->text();
	libregf_file_t* regfile = NULL;
	libregf_error_t* regerr = NULL;
	libregf_file_initialize(&regfile, &regerr);
	libregf_file_open(regfile, mboxfilepath.toStdString().c_str(), LIBREGF_OPEN_READ, &regerr);
	libregf_key_t* curkey = NULL;
	libregf_file_get_key_by_utf8_path(regfile, (uint8_t*)(keypath.toUtf8().data()), keypath.toUtf8().size(), &curkey, &regerr);
	libregf_value_t* curval = NULL;
	libregf_key_get_value(curkey, valueindex, &curval, &regerr);
        uint64_t lastwritetime = 0;
        libregf_key_get_last_written_time(curkey, &lastwritetime, &regerr);
        QString valuedata = "Last Written Time:\t" + ConvertWindowsTimeToUnixTimeUTC(lastwritetime) + " UTC\n\n";
	valuedata += "Name:\t" + ui->tablewidget->selectedItems().at(1)->text() + "\n\n";
	if(ui->tablewidget->selectedItems().at(1)->text().contains("(unnamed)"))
	{
	    valuedata += "Content\n-------\n\n";
	    valuedata += "Hex:\t0x" + ui->tablewidget->selectedItems().at(1)->text() + "\n";
	    valuedata += "Integer:\t" + QString::number(ui->tablewidget->selectedItems().at(1)->text().toInt(nullptr, 16)) + "\n";
	}
	else
	{
            QString valuetype = ui->tablewidget->selectedItems().at(2)->text();
            if(valuetype.contains("REG_SZ") || valuetype.contains("REG_EXPAND_SZ"))
            {
                valuedata += "Content:\t";
                size_t strsize = 0;
                libregf_value_get_value_utf8_string_size(curval, &strsize, &regerr);
                uint8_t valstr[strsize];
                libregf_value_get_value_utf8_string(curval, valstr, strsize, &regerr);
                valuedata += QString::fromUtf8(reinterpret_cast<char*>(valstr));
            }
            else if(valuetype.contains("REG_BINARY"))
            {
                valuedata += "Content\n-------\n\n";
                if(keypath.contains("UserAssist") && (keypath.contains("{750") || keypath.contains("{F4E") || keypath.contains("{5E6")))
                {
                    valuedata += "ROT13 Decrypted Content:\t";
                    valuedata += DecryptRot13(ui->tablewidget->selectedItems().at(1)->text()) + "\n";
                }
                else if(keypath.contains("SAM") && ui->tablewidget->selectedItems().at(1)->text().count() == 1 && ui->tablewidget->selectedItems().at(1)->text().startsWith("F"))
                {
                    size_t datasize = 0;
                    libregf_value_get_value_data_size(curval, &datasize, &regerr);
                    uint8_t data[datasize];
                    libregf_value_get_value_data(curval, data, datasize, &regerr);
                    QByteArray farray = QByteArray::fromRawData((char*)data, datasize);
                    valuedata += "Account Expiration:\t\t";
                    if(farray.mid(32,1).toHex() == "ff")
                    {
                        valuedata += "No Expiration is Set\n";
                    }
                    else
                        valuedata += ConvertWindowsTimeToUnixTimeUTC(qFromLittleEndian<uint64_t>(farray.mid(32, 8))) + " UTC\n";
                    valuedata += "Last Logon Time:\t\t" + ConvertWindowsTimeToUnixTimeUTC(qFromLittleEndian<uint64_t>(farray.mid(8, 8))) + " UTC\n";
                    valuedata += "Last Failed Login:\t\t" + ConvertWindowsTimeToUnixTimeUTC(qFromLittleEndian<uint64_t>(farray.mid(40, 8))) + " UTC\n";
                    valuedata += "Last Time Password Changed:\t" + ConvertWindowsTimeToUnixTimeUTC(qFromLittleEndian<uint64_t>(farray.mid(24, 8))) + " UTC";
                }
                else if(ui->tablewidget->selectedItems().at(1)->text().startsWith("ShutdownTime"))
                {
                    size_t datasize = 0;
                    libregf_value_get_value_data_size(curval, &datasize, &regerr);
                    uint8_t data[datasize];
                    libregf_value_get_value_data(curval, data, datasize, &regerr);
                    QByteArray valarray = QByteArray::fromRawData((char*)data, datasize);
                    valuedata += "Shutdown Time:\t" + ConvertWindowsTimeToUnixTimeUTC(qFromLittleEndian<uint64_t>(valarray)) + " UTC";
                }
            }
            else if(valuetype.contains("REG_DWORD"))
            {
                valuedata += "Content:\t";
                uint32_t dwordvalue = 0;
                libregf_value_get_value_32bit(curval, &dwordvalue, &regerr);
                if(ui->tablewidget->selectedItems().at(1)->text().toLower().contains("date"))
                    valuedata += ConvertUnixTimeToString(dwordvalue);
                else
                    valuedata += QString::number(dwordvalue);
            }
            else if(valuetype.contains("REG_DWORD_BIG_ENDIAN"))
            {
                valuedata += "Content:\t";
                uint32_t dwordvalue = 0;
                libregf_value_get_value_32bit(curval, &dwordvalue, &regerr);
                valuedata += QString::number(qFromBigEndian<uint32_t>(dwordvalue));
            }
            else if(valuetype.contains("REG_MULTI_SZ"))
            {
                valuedata += "Content\n";
                valuedata += "-------\n";
                libregf_multi_string_t* multistring = NULL;
                libregf_value_get_value_multi_string(curval, &multistring, &regerr);
                int strcnt = 0;
                libregf_multi_string_get_number_of_strings(multistring, &strcnt, &regerr);
                for(int i=0; i < strcnt; i++)
                {
                    size_t strsize = 0;
                    libregf_multi_string_get_utf8_string_size(multistring, i, &strsize, &regerr);
                    uint8_t valstr[strsize];
                    libregf_multi_string_get_utf8_string(multistring, i, valstr, strsize, &regerr);
                    valuedata += QString::fromUtf8(reinterpret_cast<char*>(valstr)) + "\n";
                }
                libregf_multi_string_free(&multistring, &regerr);
            }
            else if(valuetype.contains("REG_QWORD"))
            {
                valuedata += "Content:\t";
                uint64_t qwordvalue = 0;
                libregf_value_get_value_64bit(curval, &qwordvalue, &regerr);
                valuedata += QString::number(qwordvalue);
            }
	}
        size_t datasize = 0;
        libregf_value_get_value_data_size(curval, &datasize, &regerr);
        uint8_t data[datasize];
        libregf_value_get_value_data(curval, data, datasize, &regerr);
        QByteArray dataarray = QByteArray::fromRawData((char*)data, datasize);
        valuedata += "\n\nBinary Content\n--------------\n\n";
        int linecount = datasize / 16;
        //int remainder = datasize % 16;
        for(int i=0; i < linecount; i++)
        {
            valuedata += QString::number(i * 16, 16).rightJustified(8, '0') + "\t";
            for(int j=0; j < 16; j++)
            {
                valuedata += QString("%1").arg(data[j+i*16], 2, 16, QChar('0')).toUpper() + " ";
            }
            for(int j=0; j < 16; j++)
            {
                if(!QChar(dataarray.at(j+i*16)).isPrint())
                {
                    valuedata += ".";
                }
                else
                    valuedata += QString("%1").arg(dataarray.at(j+i*16));
            }
            valuedata += "\n";
        }
	ui->plaintext->setPlainText(valuedata);

        libregf_value_free(&curval, &regerr);
        libregf_key_free(&curkey, &regerr);
        libregf_file_close(regfile, &regerr);
        libregf_file_free(&regfile, &regerr);
        libregf_error_free(&regerr);
    }
}
*/
int WombatMail::GetRootIndex(QTreeWidgetItem* curitem)
{
    if(ui->treewidget->indexOfTopLevelItem(curitem) == -1)
	GetRootIndex(curitem->parent());
    else
	return ui->treewidget->indexOfTopLevelItem(curitem);
}

/*
void WombatMail::KeySelected(void)
{
    int itemindex = 0;
    QTreeWidgetItem* curitem = ui->treewidget->selectedItems().first();
    int rootindex = GetRootIndex(curitem);
    mboxfilepath = mboxes.at(rootindex);
    bool toplevel = false;
    QStringList pathitems;
    pathitems.clear();
    pathitems.append(curitem->text(itemindex));
    QTreeWidgetItem* parent;
    QTreeWidgetItem* child;
    child = curitem;
    while(toplevel == false)
    {
	parent = child->parent();
	if(parent == nullptr)
	    toplevel = true;
	else
	{
	    pathitems.append(parent->text(itemindex));
	    child = parent;
	}
    }
    // build path
    QString keypath = "";
    QChar sepchar = QChar(92);
    for(int i = pathitems.count() - 2; i > -1; i--)
    {
	keypath += "/" + pathitems.at(i);
    }
    keypath.replace("/", sepchar);
    // attempt to open by path...
    StatusUpdate(keypath);
    libregf_file_t* regfile = NULL;
    libregf_error_t* regerr = NULL;
    libregf_file_initialize(&regfile, &regerr);
    libregf_file_open(regfile, mboxfilepath.toStdString().c_str(), LIBREGF_OPEN_READ, &regerr);
    libregf_key_t* curkey = NULL;
    libregf_file_get_key_by_utf8_path(regfile, (uint8_t*)(keypath.toUtf8().data()), keypath.toUtf8().size(), &curkey, &regerr);
    // valid key, get values...
    int valuecount = 0;
    libregf_key_get_number_of_values(curkey, &valuecount, &regerr);
    ui->tablewidget->clear();
    ui->plaintext->setPlainText("");
    ui->tablewidget->setRowCount(valuecount);
    for(int i=0; i < valuecount; i++)
    {
	libregf_value_t* curval = NULL;
	libregf_key_get_value(curkey, i, &curval, &regerr);
	size_t namesize = 0;
	libregf_value_get_utf8_name_size(curval, &namesize, &regerr);
	uint8_t name[namesize];
	libregf_value_get_utf8_name(curval, name, namesize, &regerr);
	uint32_t type = 0;
	libregf_value_get_value_type(curval, &type, &regerr);
        QString curtagvalue = keypath + "\\";
	if(namesize == 0)
	{
	    curtagvalue += "(unnamed)";
	    ui->tablewidget->setHorizontalHeaderLabels({"Tag", "Value Name", "Value"});
	    ui->tablewidget->setItem(i, 1, new QTableWidgetItem("(unnamed)"));
	    ui->tablewidget->setItem(i, 2, new QTableWidgetItem(QString::number(type, 16)));
	}
	else
	{
	    curtagvalue += QString::fromUtf8(reinterpret_cast<char*>(name));
            QString valuetypestr = "";
	    ui->tablewidget->setHorizontalHeaderLabels({"Tag", "Value Name", "Value Type"});
	    ui->tablewidget->setItem(i, 1, new QTableWidgetItem(QString::fromUtf8(reinterpret_cast<char*>(name))));
            if(type == 0x00) // none
            {
            }
            else if(type == 0x01) // reg_sz
            {
                valuetypestr = "REG_SZ";
            }
            else if(type == 0x02) // reg_expand_sz
            {
                valuetypestr = "REG_EXPAND_SZ";
            }
            else if(type == 0x03) // reg_binary
            {
                valuetypestr = "REG_BINARY";
            }
            else if(type == 0x04) // reg_dword reg_dword_little_endian (4 bytes)
            {
                valuetypestr = "REG_DWORD";
            }
            else if(type == 0x05) // reg_dword_big_endian (4 bytes)
            {
                valuetypestr = "REG_DWORD_BIG_ENDIAN";
            }
            else if(type == 0x06) // reg_link
            {
                valuetypestr = "REG_LINK";
            }
            else if(type == 0x07) // reg_multi_sz
            {
                valuetypestr = "REG_MULTI_SZ";
            }
            else if(type == 0x08) // reg_resource_list
            {
                valuetypestr = "REG_RESOURCE_LIST";
            }
            else if(type == 0x09) // reg_full_resource_descriptor
            {
                valuetypestr = "REG_FULL_RESOURCE_DESCRIPTOR";
            }
            else if(type == 0x0a) // reg_resource_requirements_list
            {
                valuetypestr = "REG_RESOURCE_REQUIREMENTS_LIST";
            }
            else if(type == 0x0b) // reg_qword_little_endian (8 bytes)
            {
                valuetypestr = "REG_QWORD";
            }
            else
            {
            }
	    ui->tablewidget->setItem(i, 2, new QTableWidgetItem(valuetypestr));
	}
	QString tagstr = "";
        for(int j=0; j < taggeditems.count(); j++)
        {
            if(taggeditems.at(j).contains(curtagvalue))
                tagstr = taggeditems.at(j).split("|", Qt::SkipEmptyParts).first();
        }
	ui->tablewidget->setItem(i, 0, new QTableWidgetItem(tagstr));
        ui->tablewidget->resizeColumnToContents(0);
        ui->tablewidget->setCurrentCell(0, 0);
	libregf_value_free(&curval, &regerr);
    }
    libregf_key_free(&curkey, &regerr);
    libregf_file_close(regfile, &regerr);
    libregf_file_free(&regfile, &regerr);
    libregf_error_free(&regerr);
}
*/

void WombatMail::closeEvent(QCloseEvent* e)
{
    e->accept();
}

/*
void WombatMail::LoadRegistryFile(void)
{
    libregf_file_t* regfile = NULL;
    libregf_error_t* regerr = NULL;
    libregf_file_initialize(&regfile, &regerr);
    libregf_file_open(regfile, mboxfilepath.toStdString().c_str(), LIBREGF_OPEN_READ, &regerr);
    libregf_error_fprint(regerr, stderr);
    libregf_key_t* rootkey = NULL;
    libregf_file_get_root_key(regfile, &rootkey, &regerr);
    libregf_error_fprint(regerr, stderr);
    int rootsubkeycnt = 0;
    libregf_key_get_number_of_sub_keys(rootkey, &rootsubkeycnt, &regerr);
    libregf_error_fprint(regerr, stderr);
        QTreeWidgetItem* rootitem = new QTreeWidgetItem(ui->treewidget);
    rootitem->setText(0, mboxfilepath.split("/").last().toUpper() + " (" + mboxfilepath + ")");
    //rootitem->setText(0, mboxfilepath.split("/").last().toUpper());
    ui->treewidget->addTopLevelItem(rootitem);
    PopulateChildKeys(rootkey, rootitem, regerr);
    ui->treewidget->expandItem(rootitem);
    libregf_key_free(&rootkey, &regerr);
    libregf_file_close(regfile, &regerr);
    libregf_file_free(&regfile, &regerr);
    libregf_error_free(&regerr);
}
*/

/*
void WombatMail::PopulateChildKeys(libregf_key_t* curkey, QTreeWidgetItem* curitem, libregf_error_t* regerr)
{
    int subkeycount = 0;
    libregf_key_get_number_of_sub_keys(curkey, &subkeycount, &regerr);
    if(subkeycount > 0)
    {
	for(int i=0; i < subkeycount; i++)
	{
	    libregf_key_t* cursubkey = NULL;
	    libregf_key_get_sub_key(curkey, i, &cursubkey, &regerr);
	    size_t namesize = 0;
	    libregf_key_get_utf8_name_size(cursubkey, &namesize, &regerr);
	    uint8_t name[namesize];
	    libregf_key_get_utf8_name(cursubkey, name, namesize, &regerr);
	    QTreeWidgetItem* subitem = new QTreeWidgetItem(curitem);
	    subitem->setText(0, QString::fromUtf8(reinterpret_cast<char*>(name)));
	    curitem->addChild(subitem);
	    int subsubkeycount = 0;
	    libregf_key_get_number_of_sub_keys(cursubkey, &subsubkeycount, &regerr);
	    if(subsubkeycount > 0)
	    {
		PopulateChildKeys(cursubkey, subitem, regerr);
	    }
	    libregf_key_free(&cursubkey, &regerr);
	}
    }
}
*/

QString WombatMail::DecryptRot13(QString encstr)
{
    QString decstr = "";
    int i = 0;
    int strlength = 0;
    strlength = encstr.count();
    decstr = encstr;
    for(i = 0; i < strlength; i++)
    {
        decstr[i] = Rot13Char(decstr.at(i));
    }
    return decstr;
}

QChar WombatMail::Rot13Char(QChar curchar)
{
    QChar rot13char;
    if('0' <= curchar && curchar <= '4')
        rot13char = QChar(curchar.unicode() + 5);
    else if('5' <= curchar && curchar <= '9')
        rot13char = QChar(curchar.unicode() - 5);
    else if('A' <= curchar && curchar <= 'M')
        rot13char = QChar(curchar.unicode() + 13);
    else if('N' <= curchar && curchar <= 'Z')
        rot13char = QChar(curchar.unicode() - 13);
    else if('a' <= curchar && curchar <= 'm')
        rot13char = QChar(curchar.unicode() + 13);
    else if('n' <= curchar && curchar <= 'z')
        rot13char = QChar(curchar.unicode() - 13);
    else
        rot13char = curchar;
    return rot13char;
}

QString WombatMail::ConvertUnixTimeToString(uint32_t input)
{
    time_t crtimet = (time_t)input;
    QString timestr = QDateTime::fromSecsSinceEpoch(crtimet, QTimeZone::utc()).toString("MM/dd/yyyy hh:mm:ss AP");

    return timestr;
}

QString WombatMail::ConvertWindowsTimeToUnixTimeUTC(uint64_t input)
{
    uint64_t temp;
    temp = input / TICKS_PER_SECOND; //convert from 100ns intervals to seconds;
    temp = temp - EPOCH_DIFFERENCE;  //subtract number of seconds between epochs
    time_t crtimet = (time_t)temp;
    QString timestr = "";
    timestr = QDateTime::fromSecsSinceEpoch(crtimet, QTimeZone::utc()).toString("MM/dd/yyyy hh:mm:ss AP");

    return timestr;
}

void WombatMail::TagMenu(const QPoint &pt)
{
    QTableWidgetItem* currow = ui->tablewidget->itemAt(pt);
    if(ui->tablewidget->item(currow->row(), 0)->text().isEmpty())
	remtagaction->setEnabled(false);
    else
	remtagaction->setEnabled(true);
    tagmenu->exec(ui->tablewidget->mapToGlobal(pt));
}
