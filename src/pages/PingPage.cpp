#include "PingPage.hpp"

#include "Icons.hpp"
#include "Theme.hpp"
#include "widgets/ClayButton.hpp"
#include "widgets/Glass.hpp"
#include "widgets/Holo.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>

#include <algorithm>
#include <functional>
#include <utility>

namespace {

const QColor PingColors[] = {
    QColor(0x3f, 0xb6, 0xff),
    QColor(0xff, 0x8a, 0x3d),
    QColor(0xe8, 0x56, 0x3a),
    QColor(0x43, 0xd6, 0xb5),
    QColor(0xff, 0xd2, 0x3a),
};
constexpr int ColorCount = int(std::size(PingColors));

/// Five coloured dots; the selected one gets a ring.
class ColorDots : public QWidget {
public:
    static constexpr qreal Dot = 11, Gap = 12;
    std::function<void(int)> onPick;
    int selected = 0;

    ColorDots()
    {
        setFixedSize(int(ColorCount * Dot + (ColorCount - 1) * Gap + 24), 40);
        setCursor(Qt::PointingHandCursor);
        setToolTip(tr("Ping colour"));
    }

protected:
    QPointF centre(int i) const { return QPointF(12 + Dot / 2 + i * (Dot + Gap), height() / 2.0); }

    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        for (int i = 0; i < ColorCount; ++i) {
            const qreal d = (i == selected ? Dot + 3 : Dot) * (i == m_pressed ? Theme::pressedScale : 1.0);
            const QRectF dot(centre(i) - QPointF(d / 2, d / 2), QSizeF(d, d));
            if (i == selected) {
                QColor glow = PingColors[i];
                glow.setAlpha(210);
                Glass::paintGlow(p, dot, d / 2, glow);
            }
            p.setPen(Qt::NoPen);
            p.setBrush(PingColors[i]);
            p.drawEllipse(dot);
        }
    }

    int dotAt(const QPointF& pos) const
    {
        for (int i = 0; i < ColorCount; ++i) {
            if (QLineF(centre(i), pos).length() <= Dot / 2 + Gap / 2) {
                return i;
            }
        }
        return -1;
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        m_pressed = dotAt(e->position());
        update();
    }

    void mouseReleaseEvent(QMouseEvent* e) override
    {
        const int pressed = std::exchange(m_pressed, -1);
        if (pressed >= 0 && pressed == dotAt(e->position()) && onPick) {
            onPick(pressed);
        }
        update();
    }

private:
    int m_pressed = -1;
};

struct Ping {
    QString name;
    Icon icon;
    int color;
    bool visible;
};

class PingRow : public Glass::GlassCard {
public:
    explicit PingRow(const Ping& ping)
        : m_ping(ping)
    {
        m_eye = Holo::roundButton(Icon::Eye, QString());
        m_icon = new QLabel;
        m_icon->setFixedSize(40, 40);
        m_name = new QLabel(ping.name);
        m_dots = new ColorDots;

        auto* row = new QHBoxLayout(this);
        row->setContentsMargins(Theme::gap - 6, 4, Theme::gap + 4, 4);
        row->setSpacing(26);
        row->addWidget(m_eye);
        row->addWidget(m_icon);
        row->addWidget(m_name, 1);
        row->addWidget(m_dots);

        connect(m_eye, &ClayButton::clicked, this, [this] { setVisibleOnHud(!m_ping.visible); });
        m_dots->onPick = [this](int i) {
            m_ping.color = i;
            refresh();
        };
        refresh();
    }

    bool isVisibleOnHud() const { return m_ping.visible; }
    void setVisibleOnHud(bool visible)
    {
        m_ping.visible = visible;
        refresh();
    }

private:
    void refresh()
    {
        QColor c = PingColors[m_ping.color];
        if (!m_ping.visible) {
            c.setAlphaF(0.35f);
        }
        m_icon->setPixmap(Icons::pixmap(m_ping.icon, m_icon->size(), c));
        m_eye->setIconShape(m_ping.visible ? Icon::Eye : Icon::EyeOff);
        m_eye->setToolTip(m_ping.visible ? tr("Hide ping") : tr("Show ping"));
        m_name->setStyleSheet(m_ping.visible ? QStringLiteral("font-weight: bold;") : QStringLiteral("font-weight: bold; color: rgba(238, 248, 255, 90);"));
        m_dots->selected = m_ping.color;
        m_dots->update();
    }

    Ping m_ping;
    ClayButton* m_eye;
    QLabel* m_icon;
    QLabel* m_name;
    ColorDots* m_dots;
};

} // namespace

PingPage::PingPage(QWidget* parent)
    : QWidget(parent)
{
    const Ping pings[] = {
        { tr("Lifepod"), Icon::Lifepod, 0, true },
        { tr("Seamoth - SEAMOTH"), Icon::Seamoth, 3, true },
        { tr("Signal"), Icon::Signal, 2, true },
        { tr("Beacon - Kelp forest"), Icon::Beacon, 4, false },
    };

    auto* content = new QWidget;
    auto* list = new QVBoxLayout(content);
    list->setContentsMargins(0, 0, 12, 0);
    list->setSpacing(Theme::gap - 4);

    auto* toggleAll = Holo::roundButton(Icon::EyeOff, tr("Toggle all pings"));
    list->addWidget(toggleAll, 0, Qt::AlignLeft);
    list->addSpacing(4);

    QList<PingRow*> rows;
    for (const Ping& ping : pings) {
        rows.append(new PingRow(ping));
        list->addWidget(rows.last());
    }
    list->addStretch();

    connect(toggleAll, &ClayButton::clicked, this, [rows] {
        const bool anyVisible = std::any_of(rows.begin(), rows.end(), [](PingRow* r) { return r->isVisibleOnHud(); });
        for (PingRow* r : rows) {
            r->setVisibleOnHud(!anyVisible);
        }
    });

    auto* layout = Holo::pageLayout(this, tr("Ping Manager"));
    layout->addWidget(Holo::scrollArea(content), 1);
}
