#ifndef ATTACHMENTLISTMODEL_H
#define ATTACHMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>
#include <vector>
#include <optional>
#include <QHash>
#include <QByteArray>
#include <QModelIndex>
#include <QVariant>
#include "notetypes.h"

namespace Beacon::Notes
{


    class AttachmentListModel : public QAbstractListModel
    {
        Q_OBJECT
        QML_ANONYMOUS

    public:

        enum AttachmentRoles
        {
            attachmentIdRole = Qt::UserRole + 1,
            noteIdRole,
            fileNameRole,
            filePathRole,
            addedDateRole,
            fileTypeRole
        };

        explicit AttachmentListModel(QObject *parent = nullptr);

        QHash<int, QByteArray> roleNames() const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        void setAttachments(std::vector<NoteAttachment> attachments);
        bool addAttachment(const NoteAttachment& newAttachment);
        bool removeAttachment(int attachmentId);
        std::optional<NoteAttachment> getAttachmentById(int attachmentId) const;

    private:
        std::vector<NoteAttachment> m_savedAttachments;
    };
}


#endif // ATTACHMENTLISTMODEL_H
