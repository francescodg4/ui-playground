#include "ThemeDialog.hpp"

#include "ThemeRegistry.hpp"

#include <QAbstractButton>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QVBoxLayout>

#include <cmath>

namespace {

/// Miniature of a theme's look, painted from its main colours and shapes.
void paintPreview(QPainter& p, const QRectF& r, const QString& id)
{
    p.save();
    QPainterPath clip;
    clip.addRoundedRect(r, 8, 8);
    p.setClipPath(clip);
    if (id == QLatin1String("glass")) {
        p.fillRect(r, QColor(0x06, 0x18, 0x2c));
        const struct { qreal x, y, rad; QColor c; } blobs[] = {
            { 0.2, 0.3, 0.7, QColor(0x1f, 0x6f, 0xd6, 170) },
            { 0.8, 0.3, 0.6, QColor(0x12, 0xb5, 0xc9, 150) },
            { 0.6, 0.9, 0.7, QColor(0x5a, 0x3f, 0xd0, 150) },
        };
        for (const auto& b : blobs) {
            QRadialGradient g(r.left() + b.x * r.width(), r.top() + b.y * r.height(), b.rad * r.width() * 0.6);
            g.setColorAt(0, b.c);
            g.setColorAt(1, QColor(b.c.red(), b.c.green(), b.c.blue(), 0));
            p.fillRect(r, g);
        }
        p.setRenderHint(QPainter::Antialiasing);
        const QRectF bar(r.center().x() - r.width() * 0.3, r.top() + 10, r.width() * 0.6, 16);
        p.setBrush(QColor(15, 15, 15, 110));
        p.setPen(QPen(QColor(255, 255, 255, 40), 1));
        p.drawRoundedRect(bar, 8, 8);
        p.setBrush(QColor(0x3b, 0x9b, 0xff));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(QRectF(bar.left() + 4, bar.top() + 3, 22, 10), 5, 5);
        for (int i = 0; i < 2; ++i) {
            const QRectF card(r.left() + 10 + i * (r.width() - 20) / 2, r.top() + 36, (r.width() - 30) / 2, r.height() - 46);
            p.setBrush(QColor(15, 15, 15, 100));
            p.setPen(QPen(QColor(255, 255, 255, 45), 1));
            p.drawRoundedRect(card, 10, 10);
            p.setBrush(QColor(0x3d, 0x6f, 0x9e));
            p.setPen(Qt::NoPen);
            p.drawEllipse(card.center(), 11, 11);
        }
    } else if (id == QLatin1String("emerald")) {
        p.fillRect(r, QColor(0x38, 0x98, 0x88));
        for (qreal y = r.top(); y < r.bottom(); y += 12) {
            for (qreal x = r.left(); x < r.right(); x += 12) {
                p.fillRect(QRectF(x + 5, y + 5, 3, 3), QColor(0x32, 0x88, 0x7a));
            }
        }
        const QRectF box = r.adjusted(10, 30, -10, -10);
        p.fillRect(box, QColor(0x28, 0x28, 0x28));
        p.fillRect(box.adjusted(2, 2, -2, -2), QColor(0x88, 0xc8, 0x60));
        for (int i = 0; i < 6; ++i) {
            p.fillRect(QRectF(r.center().x() - 42 + i * 14, r.top() + 8, 12, 12), i == 1 ? QColor(0xf8, 0xf0, 0x90) : QColor(0xf8, 0xf8, 0xe0));
        }
        p.fillRect(QRectF(box.left() + 8, box.bottom() - 18, box.width() - 16, 12), QColor(0xf8, 0xf8, 0xe0));
        p.fillRect(QRectF(box.right() - 44, box.top() + 8, 36, box.height() - 32), QColor(0x18, 0x50, 0x48));
    } else {
        QLinearGradient metal(r.topLeft(), r.bottomLeft());
        metal.setColorAt(0, QColor(0xd8, 0xde, 0xe8));
        metal.setColorAt(1, QColor(0xa0, 0xaa, 0xb8));
        p.fillRect(r, metal);
        QLinearGradient title(r.topLeft(), QPointF(r.left(), r.top() + 12));
        title.setColorAt(0, QColor(0x18, 0x48, 0x88));
        title.setColorAt(1, QColor(0x08, 0x18, 0x30));
        p.fillRect(QRectF(r.left(), r.top(), r.width(), 12), title);
        const QRectF lcd(r.left() + 8, r.top() + 22, r.width() - 16, 30);
        p.fillRect(lcd, QColor(0x09, 0x1a, 0x38));
        for (int i = 0; i < 14; ++i) {
            const qreal h = 4 + 14 * std::abs(std::sin(i * 0.8));
            p.fillRect(QRectF(lcd.right() - 60 + i * 4, lcd.bottom() - 4 - h, 3, h), QColor(0x70, 0xb0, 0xff));
        }
        p.setPen(QColor(0x70, 0xb0, 0xff));
        QFont f = p.font();
        f.setPixelSize(16);
        f.setBold(true);
        p.setFont(f);
        p.drawText(lcd.adjusted(6, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, QStringLiteral("0:03"));
        p.setRenderHint(QPainter::Antialiasing);
        for (int i = 0; i < 5; ++i) {
            QRadialGradient g(QPointF(r.left() + 20 + i * 24, r.bottom() - 16), 11);
            g.setColorAt(0, Qt::white);
            g.setColorAt(1, QColor(0xb0, 0xb8, 0xc6));
            p.setBrush(g);
            p.setPen(QPen(QColor(0x6a, 0x74, 0x86), 1));
            p.drawEllipse(QPointF(r.left() + 20 + i * 24, r.bottom() - 16), 10, 10);
        }
    }
    p.restore();
}

/// A selectable card: preview, name and description.
class ThemeCard : public QAbstractButton {
public:
    ThemeCard(const ThemeEntry& theme, bool current)
        : m_theme(theme)
        , m_current(current)
    {
        setFixedSize(220, 210);
        setCursor(Qt::PointingHandCursor);
        setFocusPolicy(Qt::StrongFocus);
        setAttribute(Qt::WA_Hover);
        setAccessibleName(theme.name);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QRectF r = QRectF(rect()).adjusted(2, 2, -2, -2);
        const bool lit = underMouse() || hasFocus();
        p.setBrush(lit ? QColor(0x2c, 0x34, 0x46) : QColor(0x24, 0x2a, 0x38));
        p.setPen(QPen(m_current ? QColor(0x3b, 0x9b, 0xff) : lit ? QColor(0x6a, 0x78, 0x90) : QColor(0x3a, 0x42, 0x52), m_current ? 2 : 1));
        p.drawRoundedRect(r, 10, 10);
        paintPreview(p, QRectF(r.left() + 10, r.top() + 10, r.width() - 20, 110), m_theme.id);

        QFont f = font();
        f.setPointSizeF(11);
        f.setBold(true);
        p.setFont(f);
        p.setPen(QColor(0xee, 0xf2, 0xf8));
        p.drawText(QRectF(r.left() + 12, r.top() + 126, r.width() - 24, 20), Qt::AlignLeft | Qt::AlignVCenter, m_theme.name);
        if (m_current) {
            f.setPointSizeF(8);
            p.setFont(f);
            p.setPen(QColor(0x3b, 0x9b, 0xff));
            p.drawText(QRectF(r.left() + 12, r.top() + 126, r.width() - 24, 20), Qt::AlignRight | Qt::AlignVCenter, tr("CURRENT"));
        }
        f.setPointSizeF(8.5);
        f.setBold(false);
        p.setFont(f);
        p.setPen(QColor(0xa8, 0xb2, 0xc4));
        p.drawText(QRectF(r.left() + 12, r.top() + 148, r.width() - 24, r.height() - 152), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, m_theme.description);
    }

    void keyPressEvent(QKeyEvent* event) override
    {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            click();
            return;
        }
        QAbstractButton::keyPressEvent(event);
    }

private:
    ThemeEntry m_theme;
    bool m_current;
};

} // namespace

ThemeDialog::ThemeDialog(const QString& current, QWidget* parent)
    : QDialog(parent)
    , m_selected(current)
{
    setWindowTitle(tr("Select theme"));
    // a neutral look of its own, whatever theme is active
    setStyleSheet(QStringLiteral(
        "QWidget { color: #eef2f8; background: transparent; font-family: 'DejaVu Sans'; font-size: 9pt; }"
        "QLabel#title { font-size: 13pt; font-weight: bold; }"
        "QLabel#hint { color: #a8b2c4; }"
        "QPushButton { padding: 5px 16px; border-radius: 6px; border: 1px solid #4a5468; background: #2c3446; }"
        "QPushButton:hover { background: #38425a; }"));

    auto* title = new QLabel(tr("Select theme"));
    title->setObjectName(QStringLiteral("title"));
    auto* hint = new QLabel(tr("The whole interface switches at once; the choice is remembered. Shortcut: Ctrl+T."));
    hint->setObjectName(QStringLiteral("hint"));

    auto* cards = new QHBoxLayout;
    cards->setSpacing(12);
    ThemeCard* focus = nullptr;
    for (const ThemeEntry& theme : Themes::all()) {
        auto* card = new ThemeCard(theme, theme.id == current);
        connect(card, &ThemeCard::clicked, this, [this, id = theme.id] {
            m_selected = id;
            accept();
        });
        cards->addWidget(card);
        if (theme.id == current) {
            focus = card;
        }
    }

    auto* cancel = new QPushButton(tr("Cancel"));
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    auto* buttons = new QHBoxLayout;
    buttons->addWidget(hint, 1);
    buttons->addWidget(cancel);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(12);
    layout->addWidget(title);
    layout->addLayout(cards);
    layout->addLayout(buttons);
    if (focus) {
        focus->setFocus();
    }
}

void ThemeDialog::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(0x1a, 0x1f, 0x2a));
}
