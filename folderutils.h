#ifndef FOLDERUTILS_H
#define FOLDERUTILS_H
#include <QString>
#include <QDir>
#include <QStringList>


class FolderUtils
{
private:
    long m_totalNrOfFiles;
public:
    FolderUtils();
    FolderUtils(QString sourcePath);
    long totalNrOfFiles() const;
    void setTotalNrOfFiles(long totalNrOfFiles);
};

#endif // FOLDERUTILS_H
