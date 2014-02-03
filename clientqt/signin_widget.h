#pragma once

namespace Ui
{
    class sign_in;
}

class signin_widget : public QWidget
{
    Q_OBJECT

public:
    explicit signin_widget(QWidget* parent);
    ~signin_widget();

signals:
    void sign_in(const QString& server, const QString& name) const;

private slots:
    void send_sign_in() const;

private:
    Ui::sign_in* ui;
};
