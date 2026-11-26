#ifndef NOTEREPOSITORY_H
#define NOTEREPOSITORY_H

#include "notetypes.h"
#include <QString>
#include <vector>

namespace Beacon::Notes
{
    class NoteRepository
    {
    public:

        NoteRepository();

        ~NoteRepository();

        bool initialize();
        std::vector<Note> loadAllNotes() const;
        int insertNote(const Note& note);
        bool updateNote(const Note& note);
        bool deleteNote(int noteId);

        std::vector<NoteAttachment> loadAttachmentsForNote(int noteId) const;
        int insertAttachment(const NoteAttachment& attachment);
        bool deleteAttachment(int attachmentId);

    private:
        QString m_connectionName = QStringLiteral("BeaconNotesConnection");
    };

}

#endif // NOTEREPOSITORY_H
