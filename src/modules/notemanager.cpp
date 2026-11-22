#include "notemanager.h"
#include "notelistmodel.h"
#include <QDateTime>


namespace Beacon::Notes
{

NoteManager::NoteManager(QObject *parent)
    : QObject(parent), m_noteListModel(this)
    {}

bool NoteManager::createNote(const QString &noteName)
{
    Note newNote;
    QString trimmedName = noteName.trimmed();
    const QDateTime now = QDateTime::currentDateTime();

    newNote.id = m_nextNoteId;
    m_nextNoteId++;
    newNote.creationDate = newNote.updatedDate = now;

    if(trimmedName.isEmpty())
    {
        newNote.noteName = "Untitled";

    }
    else
    {
        newNote.noteName = trimmedName;
    }

    return m_noteListModel.addNote(newNote);

}

bool NoteManager::deleteNote(int noteId)
{
    if(noteId <= 0 ) return false;

    return m_noteListModel.removeNote(noteId);

}

bool NoteManager::changeNoteName(int noteId, const QString &noteName)
{
    if(noteId <= 0 || noteName.trimmed().isEmpty()) return false;

    return m_noteListModel.renameNote(noteId, noteName);


}

bool NoteManager::saveNote(int noteId)
{

}

bool NoteManager::loadNote(int noteId)
{

}



}

