/********************************************************************************
** Form generated from reading UI file 'wombatmail.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WOMBATMAIL_H
#define UI_WOMBATMAIL_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
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
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter_3;
    QTreeWidget *treewidget;
    QSplitter *splitter_2;
    QTableWidget *tablewidget;
    QSplitter *splitter;
    QPlainTextEdit *plaintext;
    QTextBrowser *textbrowser;
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
        centralwidget = new QWidget(WombatMail);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        splitter_3 = new QSplitter(centralwidget);
        splitter_3->setObjectName(QString::fromUtf8("splitter_3"));
        splitter_3->setOrientation(Qt::Horizontal);
        treewidget = new QTreeWidget(splitter_3);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treewidget->setHeaderItem(__qtreewidgetitem);
        treewidget->setObjectName(QString::fromUtf8("treewidget"));
        treewidget->setSelectionBehavior(QAbstractItemView::SelectItems);
        splitter_3->addWidget(treewidget);
        treewidget->header()->setVisible(false);
        splitter_2 = new QSplitter(splitter_3);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(splitter_2->sizePolicy().hasHeightForWidth());
        splitter_2->setSizePolicy(sizePolicy);
        splitter_2->setOrientation(Qt::Vertical);
        tablewidget = new QTableWidget(splitter_2);
        if (tablewidget->columnCount() < 5)
            tablewidget->setColumnCount(5);
        tablewidget->setObjectName(QString::fromUtf8("tablewidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(tablewidget->sizePolicy().hasHeightForWidth());
        tablewidget->setSizePolicy(sizePolicy1);
        tablewidget->setAlternatingRowColors(true);
        tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tablewidget->setColumnCount(5);
        splitter_2->addWidget(tablewidget);
        tablewidget->horizontalHeader()->setCascadingSectionResizes(true);
        tablewidget->horizontalHeader()->setStretchLastSection(true);
        tablewidget->verticalHeader()->setVisible(false);
        splitter = new QSplitter(splitter_2);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy2);
        splitter->setOrientation(Qt::Vertical);
        plaintext = new QPlainTextEdit(splitter);
        plaintext->setObjectName(QString::fromUtf8("plaintext"));
        QFont font;
        font.setFamily(QString::fromUtf8("Source Code Pro"));
        plaintext->setFont(font);
        plaintext->setReadOnly(true);
        splitter->addWidget(plaintext);
        textbrowser = new QTextBrowser(splitter);
        textbrowser->setObjectName(QString::fromUtf8("textbrowser"));
        sizePolicy1.setHeightForWidth(textbrowser->sizePolicy().hasHeightForWidth());
        textbrowser->setSizePolicy(sizePolicy1);
        splitter->addWidget(textbrowser);
        splitter_2->addWidget(splitter);
        splitter_3->addWidget(splitter_2);

        horizontalLayout->addWidget(splitter_3);

        WombatMail->setCentralWidget(centralwidget);
        menubar = new QMenuBar(WombatMail);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1022, 22));
        WombatMail->setMenuBar(menubar);
        statusbar = new QStatusBar(WombatMail);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        WombatMail->setStatusBar(statusbar);
        toolBar = new QToolBar(WombatMail);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        WombatMail->addToolBar(Qt::TopToolBarArea, toolBar);

        toolBar->addAction(actionOpenMailBox);
        toolBar->addAction(actionManageTags);
        toolBar->addAction(actionPreviewReport);
        toolBar->addAction(actionPublish);
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
        toolBar->setWindowTitle(QCoreApplication::translate("WombatMail", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WombatMail: public Ui_WombatMail {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WOMBATMAIL_H
