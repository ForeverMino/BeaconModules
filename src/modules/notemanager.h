#ifndef NOTEMANAGER_H
#define NOTEMANAGER_H

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include "notelistmodel.h"

namespace Beacon::Notes
{

    class NoteManager : public QObject
    {
        Q_OBJECT
        QML_ELEMENT
        QML_SINGLETON

        Q_PROPERTY(Beacon::Notes::NoteListModel* noteListModel READ getNoteListModel CONSTANT)

    public:

        explicit NoteManager(QObject *parent = nullptr);

        NoteListModel* getNoteListModel()  { return &m_noteListModel; }

        Q_INVOKABLE bool createNote(const QString& noteName);
        Q_INVOKABLE bool deleteNote(int noteId);
        Q_INVOKABLE bool changeNoteName (int noteId, const QString& noteName);

        bool saveNote(int noteId);
        bool loadNote(int noteId);


    private:
        NoteListModel m_noteListModel;
        int m_nextNoteId = 1;

    };


}
#endif // NOTEMANAGER_H
