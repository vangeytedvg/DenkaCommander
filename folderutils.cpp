/**
  Implentation of the FolderUtils class
  Version 0.0.1
  Author Danny Van Geyte
  */
#include "folderutils.h"

long FolderUtils::totalNrOfFiles() const
{
    return m_totalNrOfFiles;
}

void FolderUtils::setTotalNrOfFiles(long totalNrOfFiles)
{
    m_totalNrOfFiles = totalNrOfFiles;
}

QString FolderUtils::sourcePath() const
{
    return m_sourcePath;
}

void FolderUtils::setSourcePath(const QString &sourcePath)
{
    m_sourcePath = sourcePath;
}

long FolderUtils::countFiles(const QString &sourcePath)
{
    this->setSourcePath(sourcePath);
    this->countFilesInFolder(m_sourcePath);
    return this->m_totalNrOfFiles;
}

long FolderUtils::countFiles()
{
    this->countFilesInFolder(m_sourcePath);
    return this->m_totalNrOfFiles;
}

/**
 * @brief FolderUtils::countFilesInFolder
 * Recursively go through the source path and count files and folders
 * This method goes all the way down the tree hierarchy of a folder
 * @return
 */
void FolderUtils::countFilesInFolder(const QString &path)
{
    // Get the directory on the left side
    QDir sourceDir(path);
    // Get the files and copy them
    QStringList files = sourceDir.entryList(QDir::Files);
    this->m_totalNrOfFiles += files.count();

    files.clear();
    // In case of a subdirectory, use recursion
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = path + "/" + files[i];
        countFilesInFolder(srcName);
    }
    // Add the files too
    this->m_totalNrOfFiles += files.count();
}

FolderUtils::FolderUtils()
{
    // ctor
}

FolderUtils::FolderUtils(QString sourcePath)
{
    // ctor2
    this->setSourcePath(sourcePath);
}
