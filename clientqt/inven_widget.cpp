#include "stdafx.h"
#include "inven_widget.h"
#include "ui_inven2.h"

typedef QPair<int, QString> logger_line;
extern tbb::concurrent_queue<logger_line> g_logger_line_queue;

inven_widget::inven_widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::inven2_form)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
}

inven_widget::~inven_widget()
{
    delete ui;
}

void inven_widget::focusInEvent(QFocusEvent *e)
{
    QWidget::focusInEvent(e);

    g_logger_line_queue.push(logger_line(0, tr("inven_widget focus in")));
}

void inven_widget::focusOutEvent(QFocusEvent *e)
{
    QWidget::focusOutEvent(e);

    g_logger_line_queue.push(logger_line(0, tr("inven_widget focus out")));
}
