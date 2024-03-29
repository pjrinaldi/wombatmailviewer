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
    connect(ui->listwidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OpenAttach(QListWidgetItem*)), Qt::DirectConnection);
    connect(ui->actionOpenMailBox, SIGNAL(triggered()), this, SLOT(OpenMailBox()), Qt::DirectConnection);
    connect(ui->actionManageTags, SIGNAL(triggered()), this, SLOT(ManageTags()), Qt::DirectConnection);
    connect(ui->actionPreviewReport, SIGNAL(triggered()), this, SLOT(PreviewReport()), Qt::DirectConnection);
    connect(ui->actionPublish, SIGNAL(triggered()), this, SLOT(PublishReport()), Qt::DirectConnection);
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(ShowAbout()), Qt::DirectConnection);
    connect(ui->actionheader, SIGNAL(triggered(bool)), ui->headerbox, SLOT(setVisible(bool)), Qt::DirectConnection);
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
    ui->headerbox->setVisible(false);
    connect(ui->tablewidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(TagMenu(const QPoint &)), Qt::DirectConnection);
    mboxheader = QRegExp(QStringLiteral("^From .*[0-9][0-9]:[0-9][0-9]")); // kmbox regexp expression
    ui->textbrowser->setOpenExternalLinks(false);
    ui->textbrowser->setOpenLinks(false);
    ch = vmime::charset(vmime::charsets::UTF_8);
}

WombatMail::~WombatMail()
{
    delete ui;
    QDir tmpdir(QDir::tempPath() + "/wr");
    tmpdir.removeRecursively();
}

uint8_t WombatMail::MailBoxType(QString mailboxpath)
{
    //int reterr = 0;
    uint8_t mailboxtype = 0x00;
    libpff_error_t* pfferr = NULL;
    libolecf_error_t* olecerr = NULL;
    if(libpff_check_file_signature(mailboxpath.toStdString().c_str(), &pfferr)) // is pst/ost
	mailboxtype = 0x01; // PST/OST
    else if(libolecf_check_file_signature(mailboxpath.toStdString().c_str(), &olecerr)) // is msg
    {
        mailboxtype = 0x03; // MSG
    }
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
    libolecf_error_free(&olecerr);
    libpff_error_free(&pfferr);

    return mailboxtype;
}

void WombatMail::PopulatePst(QString mfpath)
{
    //qDebug() << "run populatepst...";

    int reterr = 0;
    libpff_error_t* pfferr = NULL;
    if(libpff_check_file_signature(mfpath.toStdString().c_str(), &pfferr)) // is pst/ost
    {
        // this is a pst/ost file, start processing it.
        libpff_file_t* pffile = NULL;
        reterr = libpff_file_initialize(&pffile, &pfferr);
        reterr = libpff_file_open(pffile, mfpath.toStdString().c_str(), LIBPFF_OPEN_READ, &pfferr);
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
	    QTreeWidgetItem* subdir = new QTreeWidgetItem(rootitem);
	    subdir->setText(0, QString::fromUtf8(reinterpret_cast<char*>(subname)));
	    rootitem->addChild(subdir);
	    int subdircnt = 0;
	    reterr = libpff_folder_get_number_of_sub_folders(cursubfolder, &subdircnt, &pfferr);
	    if(subdircnt > 0)
	    {
		PopulateSubFolders(mfpath, cursubfolder, subdir);
	    }
	    reterr = libpff_item_free(&cursubfolder, &pfferr);
	}
	reterr = libpff_item_free(&rootfolder, &pfferr);
        reterr = libpff_file_close(pffile, &pfferr);
        reterr = libpff_file_free(&pffile, &pfferr);
    }
    libpff_error_free(&pfferr);
}

void WombatMail::PopulateSubFolders(QString mfpath, libpff_item_t* subfolder, QTreeWidgetItem* subitem)
{
    int reterr = 0;
    libpff_error_t* pfferr = NULL;
    libpff_file_t* pffile = NULL;
    reterr = libpff_file_initialize(&pffile, &pfferr);
    reterr = libpff_file_open(pffile, mfpath.toStdString().c_str(), LIBPFF_OPEN_READ, &pfferr);
    int childcnt = 0;
    reterr = libpff_folder_get_number_of_sub_folders(subfolder, &childcnt, &pfferr);
    //qDebug() << "child folder count:" << childcnt;
    for(int i=0; i < childcnt; i++)
    {
	libpff_item_t* childfolder = NULL;
	reterr = libpff_folder_get_sub_folder(subfolder, i, &childfolder, &pfferr);
	size_t childnamesize = 0;
	reterr = libpff_folder_get_utf8_name_size(childfolder, &childnamesize, &pfferr);
	uint8_t childname[childnamesize];
	reterr = libpff_folder_get_utf8_name(childfolder, childname, childnamesize, &pfferr);
	QTreeWidgetItem* childitem = new QTreeWidgetItem(subitem);
	childitem->setText(0, QString::fromUtf8(reinterpret_cast<char*>(childname)));
	subitem->addChild(childitem);
	int childsubcnt = 0;
	reterr =libpff_folder_get_number_of_sub_folders(childfolder, &childsubcnt, &pfferr);
	if(childsubcnt > 0)
	    PopulateSubFolders(mfpath, childfolder, childitem);
	reterr = libpff_item_free(&childfolder, &pfferr);
    }
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
    ui->textbrowser->clear();
    ui->listwidget->clear();

    //ui->plaintext->setPlainText("");
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
    for(int i=0; i < msgs.count(); i++)
    {
	// MY METHOD WORKS, BUT I NOW NEED TO FIGURE OUT HOW TO HANDLE MIME PARTS
	/*
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
	*/
	/*
	// ATTEMPTING TO USE MIMETIC TO HANDLE IT
	std::istringstream tmpis(msgs.at(i).toStdString());
	//mimetic::Message tmpmsg(tmpis);
	//tmpsg(tmpis);
	//
	mimetic::MimeEntity tmpme(tmpis);
	mimetic::Header tmphead = tmpme.header();
	mimetic::Body tmpbody = tmpme.body();
	std::ostringstream tmpfrom;
	std::ostringstream tmpsubj;
        QTableWidgetItem* tmpitem = new QTableWidgetItem(QString::number(i+1));
        tmpitem->setToolTip(QString(QString::number(poslist.at(i)) + "," + QString::number(poslist.at(i+1) - poslist.at(i) - linelength.at(i))));
	ui->tablewidget->setItem(i, 0, tmpitem);
	tmpfrom << tmphead.from();
	ui->tablewidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(tmpfrom.str())));
	tmpsubj << tmphead.subject();
	ui->tablewidget->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(tmpsubj.str())));
	//tmpos << tmphead.date();
	//qDebug() << "i:" << i << "from:" << QString::fromStdString(tmpos.str());
	*/
	// ATTEMPTING TO USE VMIME TO HANDLE IT
	//vmime::charset ch(vmime::charsets::UTF_8);
	vmime::string msgdata;
	msgdata = msgs.at(i).toStdString();
	vmime::shared_ptr <vmime::message> vmsg = vmime::make_shared <vmime::message>();
	vmsg->parse(msgdata);
	vmime::messageParser vmp(vmsg);
	vmime::text vsubj = vmp.getSubject();
	vmime::mailbox vfrom = vmp.getExpeditor();
	vmime::datetime vdate = vmp.getDate();
        QTableWidgetItem* tmpitem = new QTableWidgetItem(QString::number(i+1));
        tmpitem->setToolTip(QString(QString::number(poslist.at(i)) + "," + QString::number(poslist.at(i+1) - poslist.at(i) - linelength.at(i))));
	ui->tablewidget->setItem(i, 0, tmpitem);
	ui->tablewidget->setItem(i, 2, new QTableWidgetItem(QString(QString::fromStdString(vfrom.getName().getConvertedText(ch)) + " <" + QString::fromStdString(vfrom.getEmail().toString()) + ">")));
	ui->tablewidget->setItem(i, 3, new QTableWidgetItem(QString(QString::number(vdate.getMonth()) + "/" + QString::number(vdate.getDay()) + "/" + QString::number(vdate.getYear()) + " " + QString::number(vdate.getHour()) + ":" + QString::number(vdate.getMinute()) + ":" + QString::number(vdate.getSecond()))));
	ui->tablewidget->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(vsubj.getConvertedText(ch))));
	/*
	qDebug() << "i:" << i;
	qDebug() << "from:" << QString::fromStdString(vfrom.getName().getConvertedText(ch)) << QString::fromStdString(vfrom.getEmail().toString());
	//qDebug() << "from:" << QString::fromStdString(vfrom.getConvertedText(ch));
	qDebug() << "subj:" << QString::fromStdString(vsubj.getConvertedText(ch));
	qDebug() << "date:" << vdate.getMonth() << vdate.getDay() << vdate.getYear() << vdate.getHour() << vdate.getMinute() << vdate.getSecond();
	*/
	// ATTEMPTING TO USE KMime TO HANDLE IT
	
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
		rootitem = new QTreeWidgetItem(ui->treewidget);
		rootitem->setText(0, mboxfilepath.split("/").last().toUpper() + " (" + mboxfilepath + ")");
		ui->treewidget->addTopLevelItem(rootitem);
		ui->treewidget->setCurrentItem(rootitem);
	    }
	    if(mailboxtype == 0x01) // PST/OST
	    {
                PopulatePst(mboxfilepath);
		//populate tree and table
	    }
	    else if(mailboxtype == 0x02) // MBOX
	    {
                //PopulateMbox(mboxfilepath);
		//populate table which needs to be reproducible
	    }
	}
        else if(mailboxtype == 0x03) // EML
        {
            mboxes.append(mboxfilepath);
            rootitem = new QTreeWidgetItem(ui->treewidget);
            rootitem->setText(0, mboxfilepath.split("/").last().toUpper() + " (" + mboxfilepath + ")");
            ui->treewidget->addTopLevelItem(rootitem);
            ui->treewidget->setCurrentItem(rootitem);
            //qDebug() << "it's a msg file:" << "mboxfilepath:" << mboxfilepath;
        }
	else // OTHER FILE
	{
	    // not a supported type.
	}
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
    //taggeditems.append(tagname + "|" + statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text() + "|" + ui->plaintext->toPlainText());
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
    //taggeditems.append(tagaction->iconText() + "|" + statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text() + "|" + ui->plaintext->toPlainText());

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
    QTreeWidgetItem* curitem = ui->treewidget->currentItem();
    mboxfilepath = mboxes.at(GetRootIndex(curitem));
    mailboxtype = MailBoxType(mboxfilepath);

    if(mailboxtype == 0x01) // PST/OST
    {
        QString itempath = "";
        while(curitem != NULL)
        {
            if(curitem->parent() != NULL)
                itempath += QString::number(curitem->parent()->indexOfChild(curitem)) + ",";
            else // might not need toplevel item index...
                itempath += QString::number(ui->treewidget->indexOfTopLevelItem(curitem)) + ",";
            curitem = curitem->parent();
        }
        PopulatePstFolder(mboxfilepath, itempath);
    }
    else if(mailboxtype == 0x02) // MBOX
    {
	PopulateMbox(mboxfilepath);
	//populate table which needs to be reproducible
    }
    else if(mailboxtype == 0x03) // MSG
    {
        PopulateMsg(mboxfilepath);
    }
}

void WombatMail::PopulateMsg(QString mfpath)
{
    ui->tablewidget->clear();
    ui->textbrowser->clear();
    ui->headerbrowser->clear();
    ui->listwidget->clear();
    ui->tablewidget->setHorizontalHeaderLabels({"ID", "Tag", "From", "DateTime", "Subject"});
    qDebug() << "populate message here...";
}

void WombatMail::PopulatePstFolder(QString mfpath, QString subfolders)
{
    ui->tablewidget->clear();
    ui->textbrowser->clear();
    ui->headerbrowser->clear();
    ui->listwidget->clear();
    ui->tablewidget->setHorizontalHeaderLabels({"ID", "Tag", "From", "DateTime", "Subject"});
    
    QStringList subdirlist = subfolders.split(",", Qt::SkipEmptyParts);
    //QString msgid = "";
    int reterr = 0;
    libpff_error_t* pfferr = NULL;
    if(libpff_check_file_signature(mfpath.toStdString().c_str(), &pfferr)) // is pst/ost
    {
        // this is a pst/ost file, start processing it.
        libpff_file_t* pffile = NULL;
        reterr = libpff_file_initialize(&pffile, &pfferr);
        reterr = libpff_file_open(pffile, mfpath.toStdString().c_str(), LIBPFF_OPEN_READ, &pfferr);
	libpff_error_fprint(pfferr, stderr);
        libpff_item_t* rootfolder = NULL;
        reterr = libpff_file_get_root_folder(pffile, &rootfolder, &pfferr);
        libpff_item_t* selectedfolder = NULL;
        libpff_item_t* tmpitem = NULL;
        tmpitem = rootfolder;
        for(int i = subdirlist.count() - 2; i >= 0; i--)
	{
            //msgid += subdirlist.at(i) + "-";
            libpff_item_t* curitem = NULL;
            reterr = libpff_folder_get_sub_folder(tmpitem, subdirlist.at(i).toInt(), &curitem, &pfferr);
            if(i > 0)
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
        size_t selnamesize = 0;
        reterr = libpff_folder_get_utf8_name_size(selectedfolder, &selnamesize, &pfferr);
        uint8_t selname[selnamesize];
        reterr = libpff_folder_get_utf8_name(selectedfolder, selname, selnamesize, &pfferr);
        // POPULATE THE TABLEWIDGET WITH MAIL ITEMS FOR THE FOLDER HERE...
        int msgcnt = 0;
        reterr = libpff_folder_get_number_of_sub_messages(selectedfolder, &msgcnt, &pfferr);

        if(msgcnt > 0)
        {
            ui->tablewidget->setRowCount(msgcnt);
            ui->tablewidget->setHorizontalHeaderLabels({"ID", "Tag", "From", "DateTime", "Subject"});
            for(int i=0; i < msgcnt; i++)
            {
                //msgid += QString::number(i);
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
                ui->tablewidget->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
                //ui->tablewidget->setItem(i, 0, new QTableWidgetItem(msgid));
                ui->tablewidget->setItem(i, 2, new QTableWidgetItem(QString::fromUtf8(reinterpret_cast<char*>(msgsender))));
                ui->tablewidget->setItem(i, 3, new QTableWidgetItem(ConvertWindowsTimeToUnixTimeUTC(msgtime)));
                ui->tablewidget->setItem(i, 4, new QTableWidgetItem(QString::fromUtf8(reinterpret_cast<char*>(msgsubject))));
            }

            ui->tablewidget->resizeColumnToContents(0);
            ui->tablewidget->setCurrentCell(0, 0);
        }

        reterr = libpff_item_free(&selectedfolder, &pfferr);
	reterr = libpff_item_free(&rootfolder, &pfferr);
        reterr = libpff_file_close(pffile, &pfferr);
        reterr = libpff_file_free(&pffile, &pfferr);
    }
    libpff_error_free(&pfferr);

}

void WombatMail::MailItemSelected(void)
{
    if(mailboxtype == 0x01) // PST/OST
    {
        PopulatePstEmail();
	//populate tree and table
    }
    else if(mailboxtype == 0x02) // MBOX
    {
	PopulateMboxEmail();
	//populate table which needs to be reproducible
    }
}

void WombatMail::PopulatePstEmail()
{
    QString mfpath = "";
    QTreeWidgetItem* curitem = ui->treewidget->selectedItems().first();
    mboxfilepath = mboxes.at(GetRootIndex(curitem));
    mailboxtype = MailBoxType(mboxfilepath);

    QList<int> itemlist;
    itemlist.clear();
    while(curitem != NULL)
    {
        if(curitem->parent() != NULL)
            itemlist.prepend(curitem->parent()->indexOfChild(curitem));
        else
            itemlist.prepend(ui->treewidget->indexOfTopLevelItem(curitem));
        curitem = curitem->parent();
    }
    QList<QTableWidgetItem*> curitems = ui->tablewidget->selectedItems();
    if(curitems.count() > 0)
    {
        QString msgid = curitems.at(0)->text();
        int curmsgid = msgid.split("-").last().toInt();
        int reterr = 0;
        libpff_error_t* pfferr = NULL;
        if(libpff_check_file_signature(mboxfilepath.toStdString().c_str(), &pfferr)) //if it's pst/ost
        {
            libpff_file_t* pffile = NULL;
            reterr = libpff_file_initialize(&pffile, &pfferr);
            reterr = libpff_file_open(pffile, mboxfilepath.toStdString().c_str(), LIBPFF_OPEN_READ, &pfferr);
            libpff_error_fprint(pfferr, stderr);
            libpff_item_t* rootfolder = NULL;
            reterr = libpff_file_get_root_folder(pffile, &rootfolder, &pfferr);
            libpff_item_t* selectedfolder = NULL;
            libpff_item_t* tmpitem = NULL;
            tmpitem = rootfolder;
            for(int i=1; i < itemlist.count(); i++)
            {
                libpff_item_t* curdir = NULL;
                reterr = libpff_folder_get_sub_folder(tmpitem, itemlist.at(i), &curdir, &pfferr);
                if(i < itemlist.count() - 1)
                {
                    tmpitem = curdir;
                    curdir = NULL;
                }
                else
                {
                    selectedfolder = curdir;
                    tmpitem = NULL;
                }
            }
            libpff_item_t* curmsg = NULL;
            reterr = libpff_folder_get_sub_message(selectedfolder, curmsgid, &curmsg, &pfferr);
            QString msgbodystr = "";
            /*
            size_t msghdrsize = 0;
            reterr = libpff_message_get_entry_value_utf8_string_size(curmsg, LIBPFF_ENTRY_TYPE_MESSAGE_TRANSPORT_HEADERS, &msghdrsize, &pfferr);
            uint8_t msghdr[msghdrsize];
            reterr = libpff_message_get_entry_value_utf8_string(curmsg, LIBPFF_ENTRY_TYPE_MESSAGE_TRANSPORT_HEADERS, msghdr, msghdrsize, &pfferr);
            msgbodystr = QString::fromUtf8(reinterpret_cast<char*>(msghdr));
            */
            int msgtype = 0x00; // 0x01 plaintext | 0x00 html
            size_t msgbodysize = 0;
            reterr = libpff_message_get_html_body_size(curmsg, &msgbodysize, &pfferr);
            libpff_error_fprint(pfferr, stderr);
            if(reterr < 1)
            {
                msgtype = 0x01;
                reterr = libpff_message_get_plain_text_body_size(curmsg, &msgbodysize, &pfferr);
            }

            uint8_t msgbody[msgbodysize];
            if(msgtype == 0x00) // html
            {
                //msgbodystr += "<br/><br/>";
                reterr = libpff_message_get_html_body(curmsg, msgbody, msgbodysize, &pfferr);
            }
            else if(msgtype == 0x01) // plain text
            {
                //msgbodystr += "\n\n";
                reterr = libpff_message_get_plain_text_body(curmsg, msgbody, msgbodysize, &pfferr);
            }
            msgbodystr += QString::fromUtf8(reinterpret_cast<char*>(msgbody));
            ui->textbrowser->setText(msgbodystr);
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
                /*
                uint8_t attachbuf[attachsize];
                ssize_t bufread = 0;
                bufread = libpff_attachment_data_read_buffer(curattach, attachbuf, attachsize, &pfferr);
                */
            }

            reterr = libpff_item_free(&curmsg, &pfferr);
            reterr = libpff_item_free(&selectedfolder, &pfferr);
            reterr = libpff_item_free(&rootfolder, &pfferr);
            reterr = libpff_file_close(pffile, &pfferr);
            reterr = libpff_file_free(&pffile, &pfferr);
        }
        libpff_error_free(&pfferr);
    }
}

void WombatMail::OpenAttach(QListWidgetItem* curitem)
{
    // get the attachments data, write to a tmp file and then open in a viewer or with default external application
    qDebug() << "curitem: " << curitem->text();
    qDebug() << "open attachment in viewer if mime matches, otherwise open external...";
    /*
    if(mime.contains("HTML"))
    {
    }
    else
        QDesktopServices::openUrl(QUrl(path to tmp file));
    */
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
	vmime::string msgdata;
	msgdata = msg.toStdString();
	vmime::shared_ptr <vmime::message> vmsg = vmime::make_shared <vmime::message>();
	vmsg->parse(msgdata);

        vmime::messageParser mp(vmsg);
        qDebug() << "attachment count:" << mp.getAttachmentCount();
        // handle attachments here
        // for ( int i = 0 ; i < mp.getAttachmentCount() ; ++i )
        //{
            //vmime: : shared ptr <const vmime: : attachment> att = mp.getAttachmentAt( i ) ;
            //std : : cout << ” − ” << att−>getType ( ) . generate () << std : : endl ;
        //}

        qDebug() << "text parts:" << mp.getTextPartCount();
        // handle text parts here
        // for ( int i = 0 ; i < mp. getTextPartCount() ; ++i )
        // {
            // vmime: : shared ptr <const vmime: : textPart> tp = mp. getTextPartAt( i ) ;
            // text/html
            // if (tp−>getType ( ) .getSubType() == vmime: : mediaTypes : :TEXTHTML)
            // {
                // vmime: : shared ptr <const vmime: : htmlTextPart> htp =
                // vmime: : dynamicCast <const vmime: : htmlTextPart>(tp ) ;
                // HTML text i s in tp−>getText ()
                // Plain text i s in tp−>getPlainText ()
                // Enumerate embedded objects
                // for ( int j = 0 ; j < htp−>getObjectCount() ; ++j )
                // {
                    // vmime: : shared ptr <const vmime: : htmlTextPart : : embeddedObject> obj =
                    // htp−>getObjectAt( j ) ;
                    // Identifier (Content−Id or Content−Location) i s obj−>getId ()
                    // Object data i s in obj−>getData()
                // }
            // }
            // text/plain or anything else
            // else
            // {
                // Text i s in tp−>getText ()
            // }
        // }
        /*
	//vmime::shared_ptr <vmime::header> hdr = vmsg->getHeader();
	// need to loop over all headers and then populate one per line to a string to populate plaintext
	vmime::shared_ptr <vmime::body> bdy = vmsg->getBody();
	vmime::shared_ptr <const vmime::contentHandler> cts = bdy->getContents();
	vmime::string bstr;
	vmime::utility::outputStreamStringAdapter ostr(bstr);
	cts->extract(ostr);
	// will need to actually use messageparser to get textparts and if it's text/plain, then surround it
	// in <pre></pre> so it displays correctly...
	// may need to see if the body has html content or else just set text...
	ui->textbrowser->setHtml(QString::fromStdString(bstr));
        */
	/*
	vmime::string msgdata;
	msgdata = msgs.at(i).toStdString();
	vmime::shared_ptr <vmime::message> vmsg = vmime::make_shared <vmime::message>();
	vmsg->parse(msgdata);
	*/ 
        //ui->plaintext->setPlainText(msg);
	//ui->textbrowser->setHtml(msg.replace("\n", "<br/>"));
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

/*
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
*/

/*
QString WombatMail::ConvertUnixTimeToString(uint32_t input)
{
    time_t crtimet = (time_t)input;
    QString timestr = QDateTime::fromSecsSinceEpoch(crtimet, QTimeZone::utc()).toString("MM/dd/yyyy hh:mm:ss AP");

    return timestr;
}
*/

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
