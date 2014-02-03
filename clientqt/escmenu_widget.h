#pragma once

namespace Ui
{
    class escmenu_form;
}

class escmenu_widget : public QWidget
{
    Q_OBJECT

public:
    explicit escmenu_widget(QWidget* parent);
    ~escmenu_widget();

private slots:
    void exit_now() const { emit exit_requested(); }

signals:
    void exit_requested() const;

private:
    Ui::escmenu_form* ui;
};
