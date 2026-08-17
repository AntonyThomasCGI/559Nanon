
#include "nanon/widgets/document.hpp"

#include <iostream>


using namespace nanon::widgets;


NanonDocument::NanonDocument(QObject *parent)
    : QTextDocument(parent)
{
    m_uuid = QUuid::createUuidV7();
}


NanonDocument::NanonDocument(QString uuid, QString content, int cursorPosition, QObject *parent)
    : QTextDocument(parent), m_cursorPosition(cursorPosition)
{
    m_uuid = QUuid::fromString(uuid);
    setPlainText(content);
}


NanonDocument::NanonDocument(QMap<QString, QVariant> serializedDocument, QObject *parent)
    : QTextDocument(parent)
{
    m_uuid = QUuid::fromString(serializedDocument["uuid"].toString());

    bool ok;
    m_cursorPosition = serializedDocument["cursorPosition"].toInt(&ok);
    if (!ok) {
        qWarning() << "Could not decode cursor position " << serializedDocument["cursorPosition"].toString();
    }

    QString content = serializedDocument["content"].toString();
    setPlainText(content);
}


void NanonDocument::setCursorPosition(int position)
{
    m_cursorPosition = position;
}


QMap<QString, QVariant> NanonDocument::serialize()
{
    QMap<QString, QVariant> result;
    result["uuid"] = m_uuid.toString();
    result["content"] = toPlainText();
    result["cursorPosition"] = m_cursorPosition;
    return result;
}
