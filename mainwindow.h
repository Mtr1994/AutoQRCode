#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class VisitorParser;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init();

private:
    void slot_btn_select_click();

    void slot_btn_parse_click();

private slots:
    void slot_tb_logs_costom_menu_request(const QPoint &pos);

    void slot_show_system_output_message(const QString &msg);

private:
    Ui::MainWindow *ui;

    VisitorParser *mVisitorParser = nullptr;
};
#endif // MAINWINDOW_H
