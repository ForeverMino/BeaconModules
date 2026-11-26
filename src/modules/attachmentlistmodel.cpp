#include "attachmentlistmodel.h"
#include "notetypes.h"
#include <utility>

namespace Beacon::Notes
{

AttachmentListModel::AttachmentListModel(QObject *parent) : QAbstractListModel(parent)
{

}

QHash<int, QByteArray> AttachmentListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[attachmentIdRole] = "attachmentId";
    roles[noteIdRole] = "noteId";
    roles[fileNameRole] = "fileName";
    roles[filePathRole] = "filePath";
    roles[addedDateRole] = "addedDate";
    roles[fileTypeRole] = "fileType";

    return roles;
}

int AttachmentListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;

    return static_cast<int>(m_savedAttachments.size());
}

QVariant AttachmentListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (index.row() < 0 || index.row() >= static_cast<int>(m_savedAttachments.size()))) return {};

    switch (role) {
    case attachmentIdRole:
        return m_savedAttachments.at(static_cast<size_t>(index.row())).attachmentId;
    case noteIdRole:
        return m_savedAttachments.at(static_cast<size_t>(index.row())).noteId;
    case fileNameRole:
        return m_savedAttachments.at(static_cast<size_t>(index.row())).fileName;
    case filePathRole:
        return m_savedAttachments.at(static_cast<size_t>(index.row())).filePath;
    case addedDateRole:
        return m_savedAttachments.at(static_cast<size_t>(index.row())).addedDate;
    case fileTypeRole:
        return attachmentTypeToString(m_savedAttachments.at(static_cast<size_t>(index.row())).fileType);
    default:
        return {};

    }
}


void AttachmentListModel::setAttachments(std::vector<NoteAttachment> attachments)
{
    beginResetModel();
    m_savedAttachments = std::move(attachments);
    endResetModel();
}

bool AttachmentListModel::addAttachment(const NoteAttachment &newAttachment)
{
    const int newRow = static_cast<int>(m_savedAttachments.size());

    beginInsertRows(QModelIndex(), newRow, newRow);
    m_savedAttachments.push_back(newAttachment);
    endInsertRows();

    return true;
}

bool AttachmentListModel::removeAttachment(int attachmentId)
{
    for(size_t i = 0; i < m_savedAttachments.size(); ++i)
    {
        const int row = static_cast<int>(i);

        if(m_savedAttachments.at(i).attachmentId == attachmentId)
        {
            beginRemoveRows(QModelIndex(), row, row);
            m_savedAttachments.erase(m_savedAttachments.begin() + row);
            endRemoveRows();

            return true;

        }


    }

    return false;
}

std::optional<NoteAttachment> AttachmentListModel::getAttachmentById(int attachmentId) const
{
    if(attachmentId <= 0) return std::nullopt;

    for(const auto& attachment : m_savedAttachments)
    {
        if(attachment.attachmentId == attachmentId)
        {
            return attachment;
        }
    }

    return std::nullopt;
}

}