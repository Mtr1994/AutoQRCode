#include "visitorparser.h"
#include "Public/appsignal.h"
#include "qrencode.h"

#include <thread>
#include <QFileInfo>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QStandardPaths>
#include <QDir>

// test
#include <QDebug>


VisitorParser::VisitorParser(QObject *parent)
    : QObject{parent}
{

}

void VisitorParser::parse(const QString &path)
{
    mParseFilePath = path;

    auto func = std::bind(&VisitorParser::parseOverThread, this);
    std::thread th(func);
    th.detach();
}

void VisitorParser::parseOverThread()
{
    emit AppSignal::getInstance()->sgl_show_system_output_message("开始解析人员列表文件");
    QFileInfo info(mParseFilePath);
    if (!info.exists())
    {
        emit AppSignal::getInstance()->sgl_show_system_output_message("文件不存在，请检查文件路径");
        return;
    }

    QFile file(mParseFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit AppSignal::getInstance()->sgl_show_system_output_message("无法打开文件，请检查文件路径是否正确");
        return;
    }

    QStringList listVisitor;
    while (!file.atEnd())
    {
        QString line = file.readLine();
        auto list = line.split(",");
        if (list.size() != 14) continue;
        if (!list.at(7).contains("\t")) continue;

        QString value = list.at(7);
        QString visitor = QString::number(value.remove("\t").remove("\"").toLong());
        listVisitor.append(visitor);
    }

    emit AppSignal::getInstance()->sgl_show_system_output_message(QString("解析人员列表文件完成，总计 %1 人次").arg(QString::number(listVisitor.size())));

    int scale = 50;
    QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (!QDir(desktop + "/QR_CODE").exists())
    {
        QDir dirSave(desktop);
        bool status = dirSave.mkdir("QR_CODE");
        if (!status)
        {
            emit AppSignal::getInstance()->sgl_show_system_output_message(QString("目录生成失败，请联系管理员"));
            return;
        }
    }

    int index = 0;
    for (auto &visitor : listVisitor)
    {
        index++;
        QRcode *qrcode = QRcode_encodeString(visitor.toLocal8Bit(), 0, QR_ECLEVEL_Q, QR_MODE_8, 0);
        if (qrcode == nullptr)
        {
            emit AppSignal::getInstance()->sgl_show_system_output_message(QString("编号 %1 生成失败，故障未知，请联系管理员").arg(QString("0000000000" + visitor).right(10)));
            continue;
        }

        int width = qrcode->width * scale;
        int height = qrcode->width * scale;

        QImage qr(width, height, QImage::Format_ARGB32_Premultiplied);

        QPainter painter(&qr);
        QColor background(Qt::white);
        painter.setBrush(background);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, width, height);
        QColor foreground(Qt::black);
        painter.setBrush(foreground);
        for(int y = 0; y < qrcode->width; ++y)
        {
            for(int x = 0; x < qrcode->width; ++x)
            {
                unsigned char character = qrcode->data[y * qrcode->width + x];
                if(character & 0x01)
                {
                    QRect rect(x * scale, y * scale, scale, scale);
                    painter.drawRects(&rect, 1);
                }
            }
        }

        bool status = QPixmap::fromImage(qr).save(QString("%1/QR_CODE/visitor-%2.png").arg(desktop, QString("0000000000" + visitor).right(10)));
        if (status) emit AppSignal::getInstance()->sgl_show_system_output_message(QString("编号 %1 二维码生成完毕 【序号 %2】").arg(QString("0000000000" + visitor).right(10), QString::number(index)));
        else emit AppSignal::getInstance()->sgl_show_system_output_message(QString("编号 %1 二维码保存失败").arg(QString("0000000000" + visitor).right(10)));

        QRcode_free(qrcode);
    }

    emit AppSignal::getInstance()->sgl_show_system_output_message("二维码生成完毕，请在桌面 QR_CODE 文件夹核对图像结果");
}
