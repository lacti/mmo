#pragma once

namespace Ui
{
    class inven2_form;
}

class inven_widget : public QWidget
{
    Q_OBJECT

public:
    explicit inven_widget(QWidget* parent);
    ~inven_widget();

private slots:
    void exit_now() const { emit exit_requested(); }

signals:
    void exit_requested() const;

protected:
    virtual void focusInEvent(QFocusEvent *);
    virtual void focusOutEvent(QFocusEvent *);

private:
    Ui::inven2_form* ui;
};
