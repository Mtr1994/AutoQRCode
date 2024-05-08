#ifndef VISITORPARSER_H
#define VISITORPARSER_H

#include <QObject>

class VisitorParser : public QObject
{
    Q_OBJECT
public:
    explicit VisitorParser(QObject *parent = nullptr);

    void parse(const QString &path);

private:
    void parseOverThread();

private:
    QString mParseFilePath;
    bool mParsing = false;
};

#endif // VISITORPARSER_H
