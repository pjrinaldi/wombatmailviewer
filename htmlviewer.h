#ifndef HTMLVIEWER_H
#define HTMLVIEWER_H

// Copyright 2013-2022 Pasquale J. Rinaldi, Jr.
// Distrubted under the terms of the GNU General Public License version 2

#include "ui_htmlviewer.h"
#include <QMouseEvent>

namespace Ui
{
    class HtmlViewer;
}

class HtmlViewer : public QDialog
{
    Q_OBJECT

public:
    HtmlViewer(QWidget* parent = 0);
    ~HtmlViewer();
    void ShowHtml(const QModelIndex &index);
    void LoadHtml(QString filepath);
    void SetSource(QString* repstr);

private slots:
    void HideClicked();
    void Reload();
    void GoHome();
    void GoForward();
    void GoBackward();
    
signals:
    void HideReportPreviewWindow(bool checkstate);

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::HtmlViewer* ui;
    qint64 curobjaddr;
    QByteArray htmldata;
    QString homepage;
};

Q_DECLARE_METATYPE(HtmlViewer*);

#endif // HTMLVIEWER_H
