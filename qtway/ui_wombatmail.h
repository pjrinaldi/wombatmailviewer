/********************************************************************************
** Form generated from reading UI file 'wombatmail.ui'
**
** Created by: Qt User Interface Compiler version 5.15.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WOMBATMAIL_H
#define UI_WOMBATMAIL_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WombatMail
{
public:
    QAction *actionOpenMailBox;
    QAction *actionManageTags;
    QAction *actionAbout;
    QAction *actionPreviewReport;
    QAction *actionPublish;
    QAction *actionheader;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *treewidget;
    QSplitter *splitter_3;
    QGroupBox *tablebox;
    QVBoxLayout *vert1;
    QTableWidget *tablewidget;
    QSplitter *splitter_2;
    QSplitter *splitter;
    QGroupBox *headerbox;
    QVBoxLayout *vert2;
    QTextBrowser *headerbrowser;
    QGroupBox *bodybox;
    QVBoxLayout *vert3;
    QTextBrowser *textbrowser;
    QGroupBox *attachbox;
    QVBoxLayout *vert4;
    QListWidget *listwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *WombatMail)
    {
        if (WombatMail->objectName().isEmpty())
            WombatMail->setObjectName(QString::fromUtf8("WombatMail"));
        WombatMail->resize(1022, 712);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/wombat"), QSize(), QIcon::Normal, QIcon::Off);
        WombatMail->setWindowIcon(icon);
        actionOpenMailBox = new QAction(WombatMail);
        actionOpenMailBox->setObjectName(QString::fromUtf8("actionOpenMailBox"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/bar/openfolder"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpenMailBox->setIcon(icon1);
        actionManageTags = new QAction(WombatMail);
        actionManageTags->setObjectName(QString::fromUtf8("actionManageTags"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/bar/managetags"), QSize(), QIcon::Normal, QIcon::Off);
        actionManageTags->setIcon(icon2);
        actionAbout = new QAction(WombatMail);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/help"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout->setIcon(icon3);
        actionPreviewReport = new QAction(WombatMail);
        actionPreviewReport->setObjectName(QString::fromUtf8("actionPreviewReport"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/bar/preview"), QSize(), QIcon::Normal, QIcon::Off);
        actionPreviewReport->setIcon(icon4);
        actionPublish = new QAction(WombatMail);
        actionPublish->setObjectName(QString::fromUtf8("actionPublish"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/bar/publish"), QSize(), QIcon::Normal, QIcon::Off);
        actionPublish->setIcon(icon5);
        actionheader = new QAction(WombatMail);
        actionheader->setObjectName(QString::fromUtf8("actionheader"));
        actionheader->setCheckable(true);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/bar/mail"), QSize(), QIcon::Normal, QIcon::Off);
        actionheader->setIcon(icon6);
        centralwidget = new QWidget(WombatMail);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        treewidget = new QTreeWidget(centralwidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treewidget->setHeaderItem(__qtreewidgetitem);
        treewidget->setObjectName(QString::fromUtf8("treewidget"));
        treewidget->setSelectionBehavior(QAbstractItemView::SelectItems);
        treewidget->header()->setVisible(false);

        horizontalLayout->addWidget(treewidget);

        splitter_3 = new QSplitter(centralwidget);
        splitter_3->setObjectName(QString::fromUtf8("splitter_3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(2);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(splitter_3->sizePolicy().hasHeightForWidth());
        splitter_3->setSizePolicy(sizePolicy);
        splitter_3->setOrientation(Qt::Vertical);
        tablebox = new QGroupBox(splitter_3);
        tablebox->setObjectName(QString::fromUtf8("tablebox"));
        vert1 = new QVBoxLayout(tablebox);
        vert1->setSpacing(0);
        vert1->setObjectName(QString::fromUtf8("vert1"));
        vert1->setContentsMargins(0, 0, 0, 0);
        tablewidget = new QTableWidget(tablebox);
        if (tablewidget->columnCount() < 5)
            tablewidget->setColumnCount(5);
        tablewidget->setObjectName(QString::fromUtf8("tablewidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(3);
        sizePolicy1.setHeightForWidth(tablewidget->sizePolicy().hasHeightForWidth());
        tablewidget->setSizePolicy(sizePolicy1);
        tablewidget->setMinimumSize(QSize(0, 150));
        tablewidget->setAlternatingRowColors(true);
        tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tablewidget->setColumnCount(5);
        tablewidget->horizontalHeader()->setCascadingSectionResizes(true);
        tablewidget->horizontalHeader()->setStretchLastSection(true);
        tablewidget->verticalHeader()->setVisible(false);

        vert1->addWidget(tablewidget);

        splitter_3->addWidget(tablebox);
        splitter_2 = new QSplitter(splitter_3);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Vertical);
        splitter = new QSplitter(splitter_2);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy2);
        splitter->setOrientation(Qt::Vertical);
        headerbox = new QGroupBox(splitter);
        headerbox->setObjectName(QString::fromUtf8("headerbox"));
        headerbox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headerbox->setFlat(false);
        vert2 = new QVBoxLayout(headerbox);
        vert2->setSpacing(0);
        vert2->setObjectName(QString::fromUtf8("vert2"));
        vert2->setContentsMargins(0, 0, 0, 0);
        headerbrowser = new QTextBrowser(headerbox);
        headerbrowser->setObjectName(QString::fromUtf8("headerbrowser"));

        vert2->addWidget(headerbrowser);

        splitter->addWidget(headerbox);
        bodybox = new QGroupBox(splitter);
        bodybox->setObjectName(QString::fromUtf8("bodybox"));
        vert3 = new QVBoxLayout(bodybox);
        vert3->setSpacing(0);
        vert3->setObjectName(QString::fromUtf8("vert3"));
        vert3->setContentsMargins(0, 0, 0, 0);
        textbrowser = new QTextBrowser(bodybox);
        textbrowser->setObjectName(QString::fromUtf8("textbrowser"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(textbrowser->sizePolicy().hasHeightForWidth());
        textbrowser->setSizePolicy(sizePolicy3);

        vert3->addWidget(textbrowser);

        splitter->addWidget(bodybox);
        splitter_2->addWidget(splitter);
        attachbox = new QGroupBox(splitter_2);
        attachbox->setObjectName(QString::fromUtf8("attachbox"));
        vert4 = new QVBoxLayout(attachbox);
        vert4->setSpacing(0);
        vert4->setObjectName(QString::fromUtf8("vert4"));
        vert4->setContentsMargins(0, 0, 0, 0);
        listwidget = new QListWidget(attachbox);
        listwidget->setObjectName(QString::fromUtf8("listwidget"));
        sizePolicy3.setHeightForWidth(listwidget->sizePolicy().hasHeightForWidth());
        listwidget->setSizePolicy(sizePolicy3);
        listwidget->setMaximumSize(QSize(16777215, 50));
        QPalette palette;
        QBrush brush(QColor(236, 236, 236, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        QBrush brush1(QColor(239, 239, 239, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        listwidget->setPalette(palette);
        listwidget->setFrameShape(QFrame::NoFrame);
        listwidget->setFrameShadow(QFrame::Plain);
        listwidget->setFlow(QListView::LeftToRight);
        listwidget->setProperty("isWrapping", QVariant(true));
        listwidget->setViewMode(QListView::ListMode);

        vert4->addWidget(listwidget);

        splitter_2->addWidget(attachbox);
        splitter_3->addWidget(splitter_2);

        horizontalLayout->addWidget(splitter_3);


        verticalLayout->addLayout(horizontalLayout);

        WombatMail->setCentralWidget(centralwidget);
        menubar = new QMenuBar(WombatMail);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1022, 22));
        menubar->setAcceptDrops(false);
        WombatMail->setMenuBar(menubar);
        statusbar = new QStatusBar(WombatMail);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        WombatMail->setStatusBar(statusbar);
        toolBar = new QToolBar(WombatMail);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setMovable(false);
        toolBar->setFloatable(false);
        WombatMail->addToolBar(Qt::TopToolBarArea, toolBar);

        toolBar->addAction(actionOpenMailBox);
        toolBar->addSeparator();
        toolBar->addAction(actionManageTags);
        toolBar->addAction(actionheader);
        toolBar->addSeparator();
        toolBar->addAction(actionPreviewReport);
        toolBar->addAction(actionPublish);
        toolBar->addSeparator();
        toolBar->addAction(actionAbout);

        retranslateUi(WombatMail);

        QMetaObject::connectSlotsByName(WombatMail);
    } // setupUi

    void retranslateUi(QMainWindow *WombatMail)
    {
        WombatMail->setWindowTitle(QCoreApplication::translate("WombatMail", "Wombat Mail Viewer", nullptr));
        actionOpenMailBox->setText(QCoreApplication::translate("WombatMail", "Open Mail Box", nullptr));
#if QT_CONFIG(tooltip)
        actionOpenMailBox->setToolTip(QCoreApplication::translate("WombatMail", "Open Mail Box", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionOpenMailBox->setShortcut(QCoreApplication::translate("WombatMail", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionManageTags->setText(QCoreApplication::translate("WombatMail", "Manage Tags", nullptr));
#if QT_CONFIG(tooltip)
        actionManageTags->setToolTip(QCoreApplication::translate("WombatMail", "Manage Tags", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionManageTags->setShortcut(QCoreApplication::translate("WombatMail", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
        actionAbout->setText(QCoreApplication::translate("WombatMail", "About", nullptr));
#if QT_CONFIG(tooltip)
        actionAbout->setToolTip(QCoreApplication::translate("WombatMail", "About", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionAbout->setShortcut(QCoreApplication::translate("WombatMail", "Ctrl+A", nullptr));
#endif // QT_CONFIG(shortcut)
        actionPreviewReport->setText(QCoreApplication::translate("WombatMail", "Preview Report", nullptr));
#if QT_CONFIG(tooltip)
        actionPreviewReport->setToolTip(QCoreApplication::translate("WombatMail", "Preview Report", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionPreviewReport->setShortcut(QCoreApplication::translate("WombatMail", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        actionPublish->setText(QCoreApplication::translate("WombatMail", "Publish Report", nullptr));
#if QT_CONFIG(tooltip)
        actionPublish->setToolTip(QCoreApplication::translate("WombatMail", "Publish Report", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionPublish->setShortcut(QCoreApplication::translate("WombatMail", "Ctrl+G", nullptr));
#endif // QT_CONFIG(shortcut)
        actionheader->setText(QCoreApplication::translate("WombatMail", "header", nullptr));
#if QT_CONFIG(tooltip)
        actionheader->setToolTip(QCoreApplication::translate("WombatMail", "Show/Hide Headers", nullptr));
#endif // QT_CONFIG(tooltip)
        tablebox->setTitle(QCoreApplication::translate("WombatMail", "Message List", nullptr));
        headerbox->setTitle(QCoreApplication::translate("WombatMail", "Message Headers ", nullptr));
        bodybox->setTitle(QCoreApplication::translate("WombatMail", "Message Body ", nullptr));
        attachbox->setTitle(QCoreApplication::translate("WombatMail", "Attachments", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("WombatMail", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WombatMail: public Ui_WombatMail {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WOMBATMAIL_H
