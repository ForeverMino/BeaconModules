#include "notemanager.h"
#include "notelistmodel.h"
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QUuid>
#include <QDesktopServices>
#include <optional>


namespace Beacon::Notes
{

NoteManager::NoteManager(QObject *parent)
    : QObject(parent), m_noteListModel(this), m_attachmentListModel(this)
{
    if(m_noteRepository.initialize())
    {
        for(const auto& note: m_noteRepository.loadAllNotes())
        {
            m_noteListModel.addNote(note);
        }
        m_saveTimer.setSingleShot(true);
        m_saveTimer.setInterval(400);
        connect(&m_saveTimer, &QTimer::timeout, this, &NoteManager::flushPendingSaves);
    }
}

NoteManager::~NoteManager()
{
    m_saveTimer.stop();
    flushPendingSaves();
}

bool NoteManager::createNote(const QString &noteName)
{
    Note newNote;

    QString trimmedName = noteName.trimmed();
    const QDateTime now = QDateTime::currentDateTime();


    newNote.creationDate = newNote.updatedDate = now;

    if(trimmedName.isEmpty())
    {
        newNote.noteName = "Untitled";

    }
    else
    {
        newNote.noteName = trimmedName;
    }

    const int newId = m_noteRepository.insertNote(newNote);

    if(newId <= 0 ) return false;

    newNote.noteId = newId;

    return m_noteListModel.addNote(newNote);

}

bool NoteManager::deleteNote(int noteId)
{
    if(noteId <= 0 ) return false;




    const auto attachments = m_noteRepository.loadAttachmentsForNote(noteId);

    if (!m_noteRepository.deleteNote(noteId)) return false;

    for(const auto& attachment : attachments)
    {
        QFile::remove(attachment.filePath);
    }

    m_attachmentListModel.setAttachments({});

    m_dirtyNoteIds.erase(noteId);

    return m_noteListModel.removeNote(noteId);

}

bool NoteManager::changeNoteName(int noteId, const QString &noteName)
{
    if(noteId <= 0 || noteName.trimmed().isEmpty()) return false;




    if(m_noteListModel.renameNote(noteId, noteName))
    {
        m_dirtyNoteIds.insert(noteId);
        m_saveTimer.start();
        return true;
    }

    return false;
}

void NoteManager::flushPendingSaves()
{
    for(const auto& noteId : m_dirtyNoteIds)
    {
        if (const auto note = m_noteListModel.getNoteById(noteId))
        {
           m_noteRepository.updateNote(*note);
        }

    }
    m_dirtyNoteIds.clear();
}

bool NoteManager::updateNote(int noteId, const QString &newContent)
{
    if(noteId <= 0) return false;



    if(m_noteListModel.updateNoteContent(noteId, newContent))
    {
        m_dirtyNoteIds.insert(noteId);
        m_saveTimer.start();
        return true;
    }

    return false;
}

void NoteManager::selectNote(int noteId)
{
    if(noteId <= 0) { m_attachmentListModel.setAttachments({}); return; }

    m_attachmentListModel.setAttachments(m_noteRepository.loadAttachmentsForNote(noteId));
}

QString NoteManager::attachFile(int noteId, const QUrl &fileUrl)
{
    if(noteId <= 0) return QString("");

    const QString sourcePath = fileUrl.toLocalFile();

    const QFileInfo info(sourcePath);
    if (!info.exists() || !info.isFile()) return QString("");

    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (appData.isEmpty()) return QString("");

    const QString attachmentsDir = appData + "/attachments";
    if (!QDir().mkpath(attachmentsDir)) return QString("");

    const QString uniqueName = QUuid::createUuid().toString(QUuid::WithoutBraces) + "_" + info.fileName();

    const QString destPath = attachmentsDir + "/" + uniqueName;
    if (!QFile::copy(sourcePath, destPath)) return QString("");

    NoteAttachment attachment;
    attachment.noteId = noteId;
    attachment.fileName = info.fileName();
    attachment.filePath = destPath;
    attachment.addedDate = QDateTime::currentDateTime();

    AttachmentType fileType;

    const QString ext = info.suffix().toLower();

    if(ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "webp" || ext == "gif")
    {
        fileType = AttachmentType::Image;
    }
    else if(ext == "mp4" || ext == "mkv" || ext == "webm" || ext == "mov")
    {
        fileType = AttachmentType::Video;
    }
    else if(ext == "pdf")
    {
        fileType = AttachmentType::Pdf;
    }
    else
    {
        fileType = AttachmentType::File;
    }

    attachment.fileType = fileType;

    const int newId = m_noteRepository.insertAttachment(attachment);

    if(newId <= 0)
    {
        QFile::remove(destPath);
        return QString("");
    }

    attachment.attachmentId = newId;

   m_attachmentListModel.addAttachment(attachment);

   const QString fileUrlStr = QUrl::fromLocalFile(destPath).toString();

   if(fileType == AttachmentType::Image)
   {
       return QString("<br><img src=\"%1\" width=\"320\"/><br>").arg(fileUrlStr);
   }

    return QString(" <a href=\"%1\">[%2: %3]</a> ").arg(fileUrlStr,
                                                       attachmentTypeToString(fileType),info.fileName().toHtmlEscaped());

}
bool NoteManager::removeAttachment(int attachmentId)
{

    const auto attachment = m_attachmentListModel.getAttachmentById(attachmentId);
    if (!attachment) return false;

    if (!m_noteRepository.deleteAttachment(attachmentId)) return false;

    QFile::remove(attachment->filePath);
    return m_attachmentListModel.removeAttachment(attachmentId);

}

void NoteManager::openAttachment(const QString &filePath)
{
    const QUrl url = filePath.startsWith("file:") ? QUrl(filePath) : QUrl::fromLocalFile(filePath);
    QDesktopServices::openUrl(url);
}


}

