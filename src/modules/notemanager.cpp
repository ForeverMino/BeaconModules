#include "notemanager.h"
#include "notelistmodel.h"
#include <QDateTime>


namespace Beacon::Notes
{

NoteManager::NoteManager(QObject *parent)
    : QObject(parent), m_noteListModel(this)
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

    if (!m_noteRepository.deleteNote(noteId)) return false;

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


}

