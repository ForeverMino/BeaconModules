#ifndef NOTEMANAGER_H
#define NOTEMANAGER_H

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <unordered_set>
#include "notelistmodel.h"
#include "noterepository.h"
#include "attachmentlistmodel.h"

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
        ~NoteManager() override;



        Q_PROPERTY(Beacon::Notes::AttachmentListModel* attachmentListModel READ getAttachmentListModel CONSTANT)

        Q_INVOKABLE bool createNote(const QString& noteName);
        Q_INVOKABLE bool deleteNote(int noteId);
        Q_INVOKABLE bool changeNoteName (int noteId, const QString& noteName);
        Q_INVOKABLE bool updateNote(int noteId, const QString& newContent);

        Q_INVOKABLE void selectNote(int noteId);
        Q_INVOKABLE bool attachFile(int noteId, const QUrl& fileUrl);
        Q_INVOKABLE bool removeAttachment(int attachmentId);
        Q_INVOKABLE void openAttachment(const QString& filePath);

        NoteListModel* getNoteListModel()  { return &m_noteListModel; }
        AttachmentListModel* getAttachmentListModel() { return &m_attachmentListModel; }





    private:

        NoteListModel m_noteListModel;
        NoteRepository m_noteRepository;
        AttachmentListModel m_attachmentListModel;

        QTimer m_saveTimer;

        std::unordered_set<int> m_dirtyNoteIds;

        void flushPendingSaves();

    };


}
#endif // NOTEMANAGER_H
