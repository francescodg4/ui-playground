#pragma once

#include <QDialog>

/// "Select theme": one card per theme with a small painted preview; the current one is marked.
class ThemeDialog : public QDialog {
    Q_OBJECT
public:
    /// exec() result when the user asks for the widget gallery instead.
    enum { OpenGallery = 2 };

    explicit ThemeDialog(const QString& current, QWidget* parent = nullptr);
    QString selected() const { return m_selected; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_selected;
};
