#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <QObject>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDirIterator>

class fileParser : public QObject
{
    Q_OBJECT
public:
    explicit fileParser(QObject *parent = 0);
    void clearList();

private:
    QStringList desiredFiles;
    int problemSize;

signals:
    void progressResult(const QString &status, const int &progress);
    void processingResult(const QString &status, const int &progress);
    void fetching_finished();
    void processing_finished();

public slots:
    void sepratingFiles(const QString &parameter);
    void processingFiles();
};

#endif // FILEPARSER_H
