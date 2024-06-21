#ifndef TFILETOOLS_H
#define TFILETOOLS_H

#include <QString>

class TFileTools
{
public:
    TFileTools();
    QString fileSize(const qint64 size);
    bool isFileValid(const QString filePath);
};

#endif // TFILETOOLS_H
