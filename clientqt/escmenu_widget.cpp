#include "stdafx.h"
#include "escmenu_widget.h"
#include "ui_escmenu.h"

escmenu_widget::escmenu_widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::escmenu_form)
{
    ui->setupUi(this);

    // setAttribute(Qt::WA_TranslucentBackground);

    connect(ui->exit_button, SIGNAL(clicked()), this, SLOT(exit_now()));
    connect(ui->exit_button, SIGNAL(pressed()), this, SLOT(exit_now()));

    ui->exit_button->setFocus();
}

escmenu_widget::~escmenu_widget()
{
    delete ui;
}
