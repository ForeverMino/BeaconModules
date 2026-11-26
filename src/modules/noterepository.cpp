#include "noterepository.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

namespace Beacon::Notes
{
    NoteRepository::NoteRepository()
    {}

    NoteRepository::~NoteRepository()
    {
        if(QSqlDatabase::contains(m_connectionName))
        {
            QSqlDatabase db = QSqlDatabase::database(m_connectionName);
            db.close();
        }

        QSqlDatabase::removeDatabase(m_connectionName);
    }

    bool NoteRepository::initialize()
    {
        const QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

        if(dirPath.isEmpty() || !QDir().mkpath(dirPath)) return false;

        QSqlDatabase db = QSqlDatabase::contains(m_connectionName) ? QSqlDatabase::database(m_connectionName)
                                                                   : QSqlDatabase::addDatabase("QSQLITE", m_connectionName);

        db.setDatabaseName(dirPath + "/notes.db");
        if(!db.open()) return false;

        QSqlQuery query(db);

        query.exec("PRAGMA foreign_keys = ON;");
        query.exec("PRAGMA journal_mode = WAL;");

        if(!query.exec("CREATE TABLE IF NOT EXISTS notes"
                   "("
                   "noteId INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "noteName TEXT NOT NULL,"
                   "creationDate TEXT NOT NULL,"
                   "updatedDate TEXT NOT NULL,"
                   "noteContent TEXT NOT NULL"
                   ")"))
        {
            qWarning() << query.lastError().text();
            return false;
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS note_attachments ("
                        "attachmentId INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "noteId INTEGER NOT NULL,"
                        "fileName TEXT NOT NULL,"
                        "filePath TEXT NOT NULL,"
                        "addedDate TEXT NOT NULL,"
                        "attachmentType TEXT NOT NULL,"
                        "FOREIGN KEY (noteId) REFERENCES notes(noteId) ON DELETE CASCADE)"))
        {
            qWarning() << query.lastError().text();
            return false;
        }

        return true;


    }

    std::vector<Note> NoteRepository::loadAllNotes() const
    {
        std::vector<Note> result;

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        query.prepare("SELECT noteId, noteName, creationDate, updatedDate, noteContent FROM notes ORDER BY noteId ASC ");

        if(!query.exec())
        {
            qWarning() << query.lastError().text();
            return result;
        }

        while(query.next())
        {
            int noteId = query.value(0).toInt();
            QString noteName = query.value(1).toString();
            QDateTime creationDate = QDateTime::fromString(query.value(2).toString(), Qt::ISODate);
            QDateTime updatedDate = QDateTime::fromString(query.value(3).toString(), Qt::ISODate);
            QString noteContent = query.value(4).toString();

            Note currentNote;

            currentNote.noteId = noteId;
            currentNote.noteName = noteName;
            currentNote.creationDate = creationDate;
            currentNote.updatedDate = updatedDate;
            currentNote.noteContent = noteContent;

            result.push_back(currentNote);
        }

        return result;
    }

    int NoteRepository::insertNote(const Note &note)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        query.prepare("INSERT INTO notes (noteName,creationDate,updatedDate,noteContent) VALUES (:noteName, :creationDate, "
                      ":updatedDate, :noteContent)");
        query.bindValue(":noteName", note.noteName);
        query.bindValue(":creationDate", note.creationDate.toString(Qt::ISODate));
        query.bindValue(":updatedDate", note.updatedDate.toString(Qt::ISODate));
        query.bindValue(":noteContent", note.noteContent);

        if (!query.exec())
        {
            qWarning() << query.lastError().text();
            return -1;
        }

        return query.lastInsertId().toInt();
    }

    bool NoteRepository::updateNote(const Note &note)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        query.prepare("UPDATE notes SET noteName = :noteName,"
                      "updatedDate = :updatedDate,"
                      "noteContent = :noteContent WHERE noteId = :noteId");

        query.bindValue(":noteId", note.noteId);
        query.bindValue(":noteName", note.noteName);
        query.bindValue(":updatedDate", note.updatedDate.toString(Qt::ISODate));
        query.bindValue(":noteContent", note.noteContent);

        if(!query.exec())
        {
            qWarning() << query.lastError().text();
            return false;
        }

        return true;

    }

    bool NoteRepository::deleteNote(int noteId)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        query.prepare("DELETE FROM notes WHERE noteId = :noteId");
        query.bindValue(":noteId", noteId);

        if(!query.exec())
        {
            qWarning() << query.lastError().text();
            return false;
        }
        return true;
    }

    std::vector<NoteAttachment> NoteRepository::loadAttachmentsForNote(int noteId) const
    {
        std::vector<NoteAttachment> result;

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        query.prepare("SELECT attachmentId, noteId, fileName, filePath, addedDate, attachmentType FROM note_attachments"
                      " WHERE noteId = :noteId ORDER BY attachmentId ASC");
        query.bindValue(":noteId", noteId);

        if(!query.exec())
        {
            qWarning() << query.lastError().text();
            return result;
        }

        while(query.next())
        {
            NoteAttachment attachment;

            attachment.attachmentId = query.value(0).toInt();
            attachment.noteId = query.value(1).toInt();
            attachment.fileName = query.value(2).toString();
            attachment.filePath = query.value(3).toString();
            attachment.addedDate = QDateTime::fromString(query.value(4).toString(), Qt::ISODate);
            attachment.fileType = attachmentTypeFromString(query.value(5).toString());

            result.push_back(attachment);
        }

        return result;
    }

    int NoteRepository::insertAttachment(const NoteAttachment &attachment)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        query.prepare("INSERT INTO note_attachments (noteId,fileName,filePath,addedDate, attachmentType)"
                      " VALUES (:noteId, :fileName, :filePath,:addedDate, :attachmentType)");
        query.bindValue(":noteId", attachment.noteId);
        query.bindValue(":fileName", attachment.fileName);
        query.bindValue(":filePath", attachment.filePath);
        query.bindValue(":addedDate", attachment.addedDate.toString(Qt::ISODate));
        query.bindValue(":attachmentType", attachmentTypeToString(attachment.fileType));

        if (!query.exec())
        {
            qWarning() << query.lastError().text();
            return -1;
        }

        return query.lastInsertId().toInt();
    }

    bool NoteRepository::deleteAttachment(int attachmentId)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        query.prepare("DELETE FROM note_attachments WHERE attachmentId = :attachmentId");
        query.bindValue(":attachmentId", attachmentId);

        if(!query.exec())
        {
            qWarning() << query.lastError().text();
            return false;
        }
        return true;
    }
}

