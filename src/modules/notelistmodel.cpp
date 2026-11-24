#include "notelistmodel.h"
#include <optional>

namespace Beacon::Notes
{

QHash<int, QByteArray> NoteListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[noteIdRole] = "noteId";
    roles[noteNameRole] = "noteName";
    roles[creationDateRole] = "creationDate";
    roles[updatedDateRole] = "updatedDate";
    roles[noteContentRole] = "noteContent";

    return roles;
}

int NoteListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;

    return static_cast<int>(m_savedNotes.size());

}

QVariant NoteListModel::data(const QModelIndex &index, int role) const
{


    if(!index.isValid() || (index.row() < 0 || index.row() >= static_cast<int>(m_savedNotes.size()))) return {};

    switch (role)
    {
    case noteIdRole:
        return m_savedNotes.at(static_cast<size_t>(index.row())).noteId;
        break;
    case noteNameRole:
        return m_savedNotes.at(static_cast<size_t>(index.row())).noteName;
        break;
    case creationDateRole:
        return m_savedNotes.at(static_cast<size_t>(index.row())).creationDate;
        break;
    case updatedDateRole:
        return m_savedNotes.at(static_cast<size_t>(index.row())).updatedDate;
        break;
    case noteContentRole:
        return m_savedNotes.at(static_cast<size_t>(index.row())).noteContent;
        break;
    default:
        return {};
        break;
    }
}

NoteListModel::NoteListModel(QObject *parent) : QAbstractListModel(parent)
{
}

bool NoteListModel::addNote(const Note &newNote)
{

    const int newRow = static_cast<int>(m_savedNotes.size());

    beginInsertRows(QModelIndex(), newRow, newRow);
    m_savedNotes.push_back(newNote);
    endInsertRows();

    return true;
}

bool NoteListModel::removeNote(int noteId)
{

    for(size_t i = 0; i < m_savedNotes.size(); ++i)
    {
        if(m_savedNotes.at(i).noteId == noteId)
        {
            const int row = static_cast<int>(i);

            beginRemoveRows(QModelIndex(), row, row);
            m_deletedNotes.push_back(m_savedNotes.at(i));
            m_savedNotes.erase(m_savedNotes.begin() + row);
            endRemoveRows();
            return true;
        }
    }


    return false;
}

bool NoteListModel::renameNote(int noteId, const QString &newName)
{
    for (size_t i = 0; i < m_savedNotes.size(); ++i)
    {
        if(m_savedNotes.at(i).noteId == noteId)
        {
            const int row = static_cast<int>(i);

            m_savedNotes.at(i).noteName = newName;
            m_savedNotes.at(i).updatedDate = QDateTime::currentDateTime();
            emit dataChanged(index(row), index(row), {noteNameRole, updatedDateRole});
            return true;
        }
    }
    return false;
}

bool NoteListModel::updateNoteContent(int noteId, const QString& newContent)
{
    if(noteId <= 0) return false;

    for(size_t i = 0; i < m_savedNotes.size(); ++i)
    {
        if(m_savedNotes.at(i).noteId == noteId)
        {
            const int row = static_cast<int>(i);

            m_savedNotes.at(i).noteContent = newContent;
            m_savedNotes.at(i).updatedDate = QDateTime::currentDateTime();
            emit dataChanged(index(row), index(row), {noteContentRole, updatedDateRole});
            return true;
        }
    }
    return false;
}

std::optional<Note> NoteListModel::getNoteById(int noteId) const
{

    if(noteId <= 0 ) return std::nullopt;

    for(const auto& note : m_savedNotes)
    {
        if(note.noteId == noteId)
        {

            return note;
        }
    }

    return std::nullopt;
}
}