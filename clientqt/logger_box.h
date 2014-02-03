#pragma once

class logger_box : public QPlainTextEdit
{
    Q_OBJECT

public:
    logger_box();

public slots:
    void add_chat(const QString& speaker, const QString& line);
    void add_message(const QString& line);

protected:
    void showEvent(QShowEvent* event);
};
