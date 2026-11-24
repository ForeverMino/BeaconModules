#ifndef NOTELISTMODEL_H
#define NOTELISTMODEL_H

#include <QAbstractListModel>
#include <QVariant>
#include <QObject>
#include <QDateTime>
#include <QString>
#include <QHash>
#include <QByteArray>
#include <QModelIndex>
#include <QQmlEngine>
#include <optional>
#include "notetypes.h"


namespace Beacon::Notes
{

class NoteListModel : public QAbstractListModel
{

    Q_OBJECT
    QML_ANONYMOUS

    public:
        enum NoteRoles
        {
            noteIdRole = Qt::UserRole + 1,
            noteNameRole,
            creationDateRole,
            updatedDateRole,
            noteContentRole
        };

        QHash<int, QByteArray> roleNames() const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        explicit NoteListModel(QObject *parent = nullptr);

        bool addNote(const Note& newNote);
        bool removeNote(int noteId);
        bool renameNote (int noteId, const QString& newName);
        bool updateNoteContent(int noteId, const QString& newContent);

        const std::vector<Note>& getSavedNotes() const {return m_savedNotes;}
        std::optional<Note> getNoteById(int noteId) const;

    private:

        std::vector<Note> m_savedNotes;
        std::vector<Note> m_deletedNotes;

};

}
#endif // NOTELISTMODEL_H
