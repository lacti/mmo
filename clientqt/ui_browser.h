/********************************************************************************
** Form generated from reading UI file 'browser.ui'
**
** Created: Sat Aug 3 23:46:02 2013
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BROWSER_H
#define UI_BROWSER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>
#include <QtWebKit/QWebView>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QAction *actionTake_Screenshot;
    QHBoxLayout *horizontalLayout;
    QWebView *webView;
    QPushButton *pushButton;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->resize(706, 300);
        actionTake_Screenshot = new QAction(Form);
        actionTake_Screenshot->setObjectName(QString::fromUtf8("actionTake_Screenshot"));
        horizontalLayout = new QHBoxLayout(Form);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        webView = new QWebView(Form);
        webView->setObjectName(QString::fromUtf8("webView"));
        webView->setUrl(QUrl(QString::fromUtf8("http://www.naver.com/")));

        horizontalLayout->addWidget(webView);

        pushButton = new QPushButton(Form);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout->addWidget(pushButton);


        retranslateUi(Form);
        QObject::connect(webView, SIGNAL(loadFinished(bool)), actionTake_Screenshot, SLOT(trigger()));

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
        actionTake_Screenshot->setText(QApplication::translate("Form", "Take Screenshot", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionTake_Screenshot->setToolTip(QApplication::translate("Form", "Screenshot!", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton->setText(QApplication::translate("Form", "PushButton", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BROWSER_H
