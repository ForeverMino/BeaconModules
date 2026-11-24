#ifndef NOTETYPES_H
#define NOTETYPES_H

#include <QString>
#include <QDateTime>

namespace Beacon::Notes
{
    struct Note
    {
        int noteId = 0;
        QString noteName = QString("");
        QDateTime creationDate;
        QDateTime updatedDate;
        QString noteContent = QString("");

    };
}
#endif // NOTETYPES_H
