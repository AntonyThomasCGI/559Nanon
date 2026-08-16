#pragma once

#include <QMap>
#include <QObject>
#include <QString>
#include <QTextDocument>
#include <QUuid>
#include <QVariant>


namespace nanon {
namespace widgets {


class NanonDocument : public QTextDocument
{
    Q_OBJECT

public:
    NanonDocument(QObject *parent = nullptr);
    NanonDocument(QString uuid, QString content, int cursorPosition, QObject *parent = nullptr);
    NanonDocument(QMap<QString, QVariant> serializedDocument, QObject *parent = nullptr);

    QMap<QString, QVariant> serialize();

    void setCursorPosition(int position);
    int cursorPosition() const { return m_cursorPosition; }

    QUuid uuid() const { return m_uuid; }


private:
    QUuid m_uuid;
    int m_cursorPosition = -1;
};


} // namespace widgets
} // namespace nanon
