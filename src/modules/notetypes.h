#ifndef NOTETYPES_H
#define NOTETYPES_H

#include <QString>
#include <QDateTime>

namespace Beacon::Notes
{

    enum class AttachmentType
    {
        Image,
        Pdf,
        Video,
        File
    };

    inline QString attachmentTypeToString(AttachmentType type)
    {
        switch(type)
        {
        case AttachmentType::Image:
            return QString("Image");
        case AttachmentType::Pdf:
            return QString("Pdf");
        case AttachmentType::Video:
            return QString("Video");
        case AttachmentType::File:
            return QString("File");
        default:
            return QString("File");
        }
    }
    inline AttachmentType attachmentTypeFromString(const QString& str)
    {
        const QString lower = str.toLower();

        if(lower == "image")
        {
            return AttachmentType::Image;
        }
        else if(lower == "pdf")
        {
            return AttachmentType::Pdf;
        }
        else if(lower == "video")
        {
            return AttachmentType::Video;
        }

        return AttachmentType::File;
    }

    struct Note
    {
        int noteId = 0;
        QString noteName = QString("");
        QDateTime creationDate;
        QDateTime updatedDate;
        QString noteContent = QString("");
    };

    struct NoteAttachment
    {
        int attachmentId = 0;
        int noteId = 0;
        QString fileName = QString("");
        QString filePath = QString("");
        QDateTime addedDate;
        AttachmentType fileType = AttachmentType::File;

    };


}
#endif // NOTETYPES_H
