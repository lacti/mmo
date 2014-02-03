#include "stdafx.h"
#include "logger_box.h"

logger_box::logger_box()
{
    //setReadOnly(true);
    setUndoRedoEnabled(false);
    setStyleSheet("color: rgb(235, 235, 235); background-color: rgba(0, 30, 0, 180);");

    setFixedWidth(500);
    setFixedHeight(100);
    //setLineWrapMode(QPlainTextEdit::NoWrap);

    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    appendPlainText(tr("Welcome!"));
}

void logger_box::add_chat(const QString& speaker, const QString& line)
{
    add_message(QString("[%1] : %2").arg(speaker).arg(line));
}

void logger_box::add_message(const QString& line)
{
    appendPlainText(line);
    ensureCursorVisible();
}

void logger_box::showEvent(QShowEvent* event)
{
    ensureCursorVisible();

    QPlainTextEdit::showEvent(event);
}
