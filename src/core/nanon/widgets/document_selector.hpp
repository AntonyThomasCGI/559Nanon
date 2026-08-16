#pragma once

#include <QAbstractListModel>
#include <QListView>
#include <QVariant>
#include <QModelIndex>
#include <QTextDocument>


namespace nanon {
namespace widgets {


class NanonDocumentView : public QListView
{
    Q_OBJECT

public:
    NanonDocumentView(QWidget *parent = nullptr) : QListView(parent) {}

};


class NanonDocumentModel : public QAbstractListModel
{
    Q_OBJECT

public:
    NanonDocumentModel(QList<QTextDocument*> documents, QObject *parent = nullptr);
    virtual ~NanonDocumentModel() = default;

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addDocument(QTextDocument *document);

private:
    QList<QTextDocument*> m_documents;
};



}  // namespace widgets
}  // namespace nanon
