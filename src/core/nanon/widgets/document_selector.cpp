
#include "nanon/widgets/document_selector.hpp"


using namespace nanon::widgets;


NanonDocumentModel::NanonDocumentModel(QList<QTextDocument*> documents, QObject *parent)
    : QAbstractListModel(parent), m_documents(documents)
{

}


int NanonDocumentModel::rowCount(const QModelIndex &parent) const
{
    return m_documents.length();
}

QVariant NanonDocumentModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
        case (Qt::DisplayRole):
            return QString("A Document");

        default:
            return QVariant();
    }
}


void NanonDocumentModel::addDocument(QTextDocument *document)
{
    m_documents.push_back(document);
}
