#pragma once

#include "nanon/style/theme.hpp"
#include "nanon/widgets/document.hpp"

#include <QSettings>
#include <QtCore/QObject>

#include <memory>


namespace nanon {


class NanonSession : public QObject
{
    Q_OBJECT

public:
    NanonSession();
    ~NanonSession() = default;

    style::NanonTheme* currentTheme();
    void setCurrentTheme(style::NanonTheme *theme);

    void saveDocument(widgets::NanonDocument *document);
    QList<QMap<QString, QVariant>> loadDocuments();

signals:
    void themeChanged(style::NanonTheme*);

private:
    std::unique_ptr<QSettings> m_settings;

};


}  // namespace nanon
