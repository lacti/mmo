#include "stdafx.h"
#include "signin_widget.h"
#include "ui_signin.h"

signin_widget::signin_widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::sign_in)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);

    QSettings settings;
    ui->user_name->setText(settings.value("last/signin").toString());

    QStringList sl = settings.value("last/serverlist").toStringList();
    sl.removeDuplicates();
    bool cleared = false;
    for (int i = 0; i < sl.size(); ++i)
    {
        if (sl[i].length() > 0)
        {
            if (cleared == false)
            {
                ui->server_name->clear();
                cleared = true;
            }

            ui->server_name->insertItem(i, sl[i]);
        }
    }

    connect(ui->sign_in_button, SIGNAL(clicked()), this, SLOT(send_sign_in()));
    connect(ui->user_name, SIGNAL(returnPressed()), this, SLOT(send_sign_in()));

    ui->user_name->setFocus();
}

signin_widget::~signin_widget()
{
    delete ui;
}

void signin_widget::send_sign_in() const
{
    QSettings settings;
    settings.setValue("last/signin", ui->user_name->text());

    ui->server_name->insertItem(0, ui->server_name->currentText());
    
    QStringList sl;
    for (int i = 0; i < ui->server_name->count(); ++i)
    {
        sl.append( ui->server_name->itemText(i) );
    }
    settings.setValue("last/serverlist", sl);

    emit sign_in(ui->server_name->currentText(), ui->user_name->text());
}
