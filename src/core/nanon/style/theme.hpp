#pragma once

#include <QTextCharFormat>
#include <QtGui/QColor>
#include <QtCore/QString>
#include <QtCore/QVector>


namespace nanon {
namespace style {


struct TokenColor
{
    QString name;
    QVector<QString> scopes;
    QTextCharFormat format;
};


class NanonTheme
{
public:
    NanonTheme();

    QString getName() const { return m_name; };

    void loadFromFile(QString fileName);
    void loadFromData(QMap<QString, QVariant> rawTheme);

    /** An iterator for TokenColors */
    class TokenColorRange {
    public:
        TokenColorRange(QVector<TokenColor>::const_iterator begin,
                        QVector<TokenColor>::const_iterator end)
            : m_begin(begin), m_end(end) {}

        QVector<TokenColor>::const_iterator begin() const { return m_begin; }
        QVector<TokenColor>::const_iterator end() const { return m_end; }

    private:
        QVector<TokenColor>::const_iterator m_begin;
        QVector<TokenColor>::const_iterator m_end;
    };

    TokenColorRange tokenColors() const {
        return TokenColorRange(m_tokenColors.cbegin(), m_tokenColors.cend());
    }

    QColor getColor(QString key);


private:
    QString m_name;
    QVector<TokenColor> m_tokenColors;
    QMap<QString, QColor> m_colors;
};


}  // namespace style
}  // namespace nanon

