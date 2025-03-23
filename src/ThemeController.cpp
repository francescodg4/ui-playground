#include "ThemeController.hpp"

#include "PdaShell.hpp"
#include "ThemeDialog.hpp"
#include "WidgetGallery.hpp"

#include <QApplication>
#include <QShortcut>
#include <QStyle>

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
    connect(new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_G), window), &QShortcut::activated, this, [this] { showGallery(); });

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
    const int result = dialog.exec();
    if (result == ThemeDialog::OpenGallery) {
        showGallery();
    } else if (result == QDialog::Accepted && apply(dialog.selected())) {
        Themes::save(dialog.selected());
    }
}

void ThemeController::showGallery(const QString& id)
{
    if (m_window) {
        m_window->setAttribute(Qt::WA_DeleteOnClose, false);
        m_window->hide();
        m_window->deleteLater();
        m_window = nullptr;
    }
    // the gallery is drawn by the themes' widget styles alone
    qApp->setStyleSheet(QString());
    if (!m_gallery) {
        m_gallery = new WidgetGallery;
        m_gallery->setAttribute(Qt::WA_DeleteOnClose);
        connect(m_gallery, &WidgetGallery::openPdaRequested, this, &ThemeController::leaveGallery);
    }
    m_gallery->setTheme(Themes::find(id) ? id : !m_current.isEmpty() ? m_current : Themes::saved());
    m_gallery->show();
    m_gallery->raise();
    m_gallery->activateWindow();
}

void ThemeController::leaveGallery(const QString& id)
{
    // the PDA draws with Fusion under the theme's style sheet
    QApplication::setStyle(QStringLiteral("Fusion"));
    QApplication::setPalette(QApplication::style()->standardPalette());
    if (!apply(id)) {
        return;
    }
    m_window->show();
    if (m_gallery) {
        m_gallery->setAttribute(Qt::WA_DeleteOnClose, false);
        m_gallery->hide();
        m_gallery->deleteLater();
        m_gallery = nullptr; // never reuse it: the PDA's style sheet may repolish it before it goes
    }
}

void ThemeController::setAnimationsEnabled(bool enabled)
{
    m_animations = enabled;
    if (m_window) {
        m_window->setAnimationsEnabled(enabled && !m_options.reducedMotion);
    }
}
