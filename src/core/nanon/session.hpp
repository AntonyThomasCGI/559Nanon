#pragma once

#include "nanon/style/theme.hpp"

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

    void saveEditorContent(int index, const QString &editorText);
    QList<QString> loadEditorContent();

signals:
    void themeChanged(style::NanonTheme*);

private:
    std::unique_ptr<QSettings> m_settings;

};


}  // namespace nanon
