#include "folderutils.h"

long FolderUtils::totalNrOfFiles() const
{
    return m_totalNrOfFiles;
}

void FolderUtils::setTotalNrOfFiles(long totalNrOfFiles)
{
    m_totalNrOfFiles = totalNrOfFiles;
}

FolderUtils::FolderUtils()
{

}
