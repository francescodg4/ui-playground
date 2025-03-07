#include "ThemeController.hpp"

#include "PdaShell.hpp"
#include "ThemeDialog.hpp"

#include <QApplication>
#include <QShortcut>

ThemeController::ThemeController(PhotoLibrary* photos, const ThemeOptions& options, QObject* parent)
    : QObject(parent)
    , m_photos(photos)
    , m_options(options)
{
}

bool ThemeController::apply(const QString& id)
{
    const ThemeEntry* theme = Themes::find(id);
    if (!theme || (id == m_current && m_window)) {
        return theme != nullptr;
    }
    QPointer<PdaShell> old = m_window;

    qApp->setStyleSheet(theme->styleSheet());
    QApplication::setFont(theme->font());
    PdaShell* window = theme->create(m_photos, m_options);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->setAnimationsEnabled(m_animations && !m_options.reducedMotion);
    connect(window, &PdaShell::selectThemeRequested, this, &ThemeController::selectTheme);
    connect(new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), window), &QShortcut::activated, this, &ThemeController::selectTheme);

    if (old) {
        window->setGeometry(old->geometry());
        window->setPage(old->currentPage());
        window->setWindowState(old->windowState());
        window->show();
        old->setAttribute(Qt::WA_DeleteOnClose, false);
        old->hide();
        old->deleteLater();
    } else {
        window->resize(1100, 640);
    }
    m_window = window;
    m_current = id;
    return true;
}

void ThemeController::selectTheme()
{
    ThemeDialog dialog(m_current, m_window);
    if (dialog.exec() == QDialog::Accepted && apply(dialog.selected())) {
        Themes::save(dialog.selected());
    }
}

void ThemeController::setAnimationsEnabled(bool enabled)
{
    m_animations = enabled;
    if (m_window) {
        m_window->setAnimationsEnabled(enabled && !m_options.reducedMotion);
    }
}
