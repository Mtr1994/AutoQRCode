#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Public/appsignal.h"
#include "Parser/visitorparser.h"

#include <QFileDialog>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();

    setWindowTitle("夏日展馆 二维码自动生成软件 V1.1.0.1451  【 邓静 】");
}

MainWindow::~MainWindow()
{
    delete mVisitorParser;
    delete ui;
}

void MainWindow::init()
{
    connect(ui->tbLogs, &QTextEdit::customContextMenuRequested, this, &MainWindow::slot_tb_logs_costom_menu_request);
    connect(ui->btnSelect, &QPushButton::clicked, this, &MainWindow::slot_btn_select_click);
    connect(ui->btnParse, &QPushButton::clicked, this, &MainWindow::slot_btn_parse_click);

    connect(AppSignal::getInstance(), &AppSignal::sgl_show_system_output_message, this, &MainWindow::slot_show_system_output_message);

    mVisitorParser = new VisitorParser;

    // 固定大小
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
}

void MainWindow::slot_btn_select_click()
{
    QString filter;
    QString path = QFileDialog::getOpenFileName(this, "打开文件", "", "支持的文件 (*.csv);;文本文件 (*.csv)", &filter);
    if (path.isEmpty()) return;

    ui->tbPath->setText(path);
}

void MainWindow::slot_btn_parse_click()
{
    ui->tbPath->clearFocus();
    if (nullptr == mVisitorParser) return;
    QString path = ui->tbPath->text().trimmed();
    mVisitorParser->parse(path);
}

void MainWindow::slot_tb_logs_costom_menu_request(const QPoint &pos)
{
    Q_UNUSED(pos);
    QMenu menu(this);
    menu.setWindowFlags(Qt::NoDropShadowWindowHint | menu.windowFlags() | Qt::FramelessWindowHint);
    menu.setAttribute(Qt::WA_TranslucentBackground);
    QAction actionCopy("复制");
    // 选中才能复制
    connect(&actionCopy, &QAction::triggered, this, [this]() { ui->tbLogs->copy(); });
    QAction actionClear("清理");
    connect(&actionClear, &QAction::triggered, this, [this]() { ui->tbLogs->clear(); });

    menu.addAction(&actionCopy);
    menu.addAction(&actionClear);

    menu.exec(QCursor::pos());
}

void MainWindow::slot_show_system_output_message(const QString &msg)
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->tbLogs->append(QString("%1：%2").arg(time, msg));
}

