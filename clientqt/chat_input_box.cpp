#include "stdafx.h"
#include "chat_input_box.h"

chat_input_box::chat_input_box()
{
    setFrame(false);
    setEnabled(false);
    setReadOnly(true);
    setContextMenuPolicy(Qt::NoContextMenu);
}

void chat_input_box::keyPressEvent(QKeyEvent * event)
{   
    QLineEdit::keyPressEvent(event);

    if (event->key() == Qt::Key_Escape && event->isAutoRepeat() == false)
    {   
        setEnabled(false);
        setReadOnly(true);
        
        event->accept();

        return;
    }
    else if (event->key() == Qt::Key_Return)
    {
        QString trimmed = text().trimmed();
        if (trimmed.length() > 0)
        {
            emit send_chat(trimmed);
        }

        if (text().length() == 0)
        {
            //setEnabled(false);
            //setReadOnly(true);
        }
        else
        {
            clear();
        }
    }
    else if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
    {
        event->accept();
    }

}

void chat_input_box::keyReleaseEvent(QKeyEvent * event)
{
    QLineEdit::keyReleaseEvent(event);

    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
    {
        event->accept();
    }
}
