#pragma once

class chat_input_box : public QLineEdit
{
    Q_OBJECT

public:
    chat_input_box();

protected:
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);

signals:
    void send_chat(const QString& line);
    
};
