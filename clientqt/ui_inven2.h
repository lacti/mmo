/********************************************************************************
** Form generated from reading UI file 'inven2.ui'
**
** Created: Sat Aug 3 23:46:02 2013
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INVEN2_H
#define UI_INVEN2_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_inven2_form
{
public:
    QHBoxLayout *horizontalLayout;
    QListWidget *list;

    void setupUi(QWidget *inven2_form)
    {
        if (inven2_form->objectName().isEmpty())
            inven2_form->setObjectName(QString::fromUtf8("inven2_form"));
        inven2_form->resize(382, 69);
        horizontalLayout = new QHBoxLayout(inven2_form);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        list = new QListWidget(inven2_form);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/icons/001-Weapon01.png"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem = new QListWidgetItem(list);
        __qlistwidgetitem->setIcon(icon);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/icons/033-Item02.png"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem1 = new QListWidgetItem(list);
        __qlistwidgetitem1->setIcon(icon1);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/icons/005-Weapon05.png"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem2 = new QListWidgetItem(list);
        __qlistwidgetitem2->setIcon(icon2);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/icons/022-Potion02.png"), QSize(), QIcon::Normal, QIcon::Off);
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::NoBrush);
        QBrush brush1(QColor(72, 0, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        QListWidgetItem *__qlistwidgetitem3 = new QListWidgetItem(list);
        __qlistwidgetitem3->setFont(font);
        __qlistwidgetitem3->setBackground(brush1);
        __qlistwidgetitem3->setForeground(brush);
        __qlistwidgetitem3->setIcon(icon3);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/icons/015-Body03.png"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem4 = new QListWidgetItem(list);
        __qlistwidgetitem4->setIcon(icon4);
        list->setObjectName(QString::fromUtf8("list"));
        list->setFocusPolicy(Qt::NoFocus);
        list->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 255, 255, 0);"));
        list->setFrameShape(QFrame::NoFrame);
        list->setLineWidth(0);
        list->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        list->setProperty("showDropIndicator", QVariant(false));
        list->setDragDropMode(QAbstractItemView::NoDragDrop);
        list->setIconSize(QSize(24, 24));
        list->setMovement(QListView::Static);
        list->setViewMode(QListView::IconMode);

        horizontalLayout->addWidget(list);


        retranslateUi(inven2_form);

        QMetaObject::connectSlotsByName(inven2_form);
    } // setupUi

    void retranslateUi(QWidget *inven2_form)
    {
        inven2_form->setWindowTitle(QApplication::translate("inven2_form", "Form", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = list->isSortingEnabled();
        list->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = list->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("inven2_form", "\354\271\274", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem1 = list->item(1);
        ___qlistwidgetitem1->setText(QApplication::translate("inven2_form", "\355\216\270\354\247\200", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem2 = list->item(2);
        ___qlistwidgetitem2->setText(QApplication::translate("inven2_form", "\355\231\224\354\202\264", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem3 = list->item(3);
        ___qlistwidgetitem3->setText(QApplication::translate("inven2_form", "\354\235\264\353\246\204\354\235\264 \353\247\244\354\232\260 \352\270\264 \355\217\254\354\205\230", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        ___qlistwidgetitem3->setStatusTip(QApplication::translate("inven2_form", "\354\235\264\352\261\264 \353\255\220\354\247\200?", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_TOOLTIP
        ___qlistwidgetitem3->setToolTip(QApplication::translate("inven2_form", "<html><head/><body><p><span style=\" font-size:12pt;\">\354\235\264 \354\225\275\354\235\204 \353\250\271\354\234\274\353\251\264 \353\247\244\354\232\260 \352\260\225\353\240\245\355\225\264\354\247\220</span><img src=\":/icons/icons/020-Accessory05.png\"/></p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        ___qlistwidgetitem3->setWhatsThis(QApplication::translate("inven2_form", "\354\236\220\354\236\220\354\236\220", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        QListWidgetItem *___qlistwidgetitem4 = list->item(4);
        ___qlistwidgetitem4->setText(QApplication::translate("inven2_form", "\352\260\221\354\230\267", 0, QApplication::UnicodeUTF8));
        list->setSortingEnabled(__sortingEnabled);

    } // retranslateUi

};

namespace Ui {
    class inven2_form: public Ui_inven2_form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INVEN2_H
