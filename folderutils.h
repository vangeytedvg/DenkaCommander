#ifndef FOLDERUTILS_H
#define FOLDERUTILS_H
#include <QString>
#include <QDir>
#include <QStringList>


class FolderUtils
{
private:
    long m_totalNrOfFiles = 0;
    QString m_sourcePath = "";
    void countFilesInFolder(const QString &path);

public:
    FolderUtils();
    FolderUtils(QString sourcePath);
    long totalNrOfFiles() const;
    void setTotalNrOfFiles(long totalNrOfFiles);
    QString sourcePath() const;
    void setSourcePath(const QString &sourcePath);
    long countFiles(const QString &sourcePath);
    long countFiles();
};

#endif // FOLDERUTILS_H
