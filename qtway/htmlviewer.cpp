#include "htmlviewer.h"

// Copyright 2013-2022 Pasquale J. Rinaldi, Jr.
// Distrubted under the terms of the GNU General Public License version 2

HtmlViewer::HtmlViewer(QWidget* parent) : QDialog(parent), ui(new Ui::HtmlViewer)
{
    ui->setupUi(this);
    setWindowTitle("Report Preview");
    homepage = "";

    ui->homebutton->setVisible(false);
    connect(ui->homebutton, SIGNAL(clicked()), this, SLOT(GoHome()));
    connect(ui->backbutton, SIGNAL(clicked()), ui->textbrowser, SLOT(backward()));
    connect(ui->forbutton, SIGNAL(clicked()), ui->textbrowser, SLOT(forward()));
    connect(ui->textbrowser, SIGNAL(backwardAvailable(bool)), ui->backbutton, SLOT(setEnabled(bool)));
    connect(ui->textbrowser, SIGNAL(forwardAvailable(bool)), ui->forbutton, SLOT(setEnabled(bool)));
}

HtmlViewer::~HtmlViewer()
{
    delete ui;
    this->close();
}

void HtmlViewer::SetSource(QString* repstr)
{
    if(homepage.isEmpty())
        homepage = *repstr;
    ui->textbrowser->setHtml(*repstr);
}

void HtmlViewer::GoHome()
{
    ui->textbrowser->setHtml(homepage);
}

void HtmlViewer::GoBackward()
{
    ui->textbrowser->backward();
}

void HtmlViewer::GoForward()
{
    ui->textbrowser->forward();
}

void HtmlViewer::HideClicked()
{
    this->close();
}

void HtmlViewer::ShowHtml(const QModelIndex &index)
{
    this->show();
}

void HtmlViewer::LoadHtml(QString filepath)
{
    ui->textbrowser->setSource(QUrl::fromLocalFile(filepath));
    this->show();
}

void HtmlViewer::Reload()
{
    ui->textbrowser->reload();
}

void HtmlViewer::closeEvent(QCloseEvent* e)
{
    emit HideReportPreviewWindow(false);
    e->accept();
}
