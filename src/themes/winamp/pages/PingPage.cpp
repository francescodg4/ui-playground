#include "PingPage.hpp"

#include "Icons.hpp"
#include "themes/winamp/Theme.hpp"
#include "themes/winamp/widgets/Metal.hpp"
#include "themes/winamp/widgets/MetalWidgets.hpp"
#include "themes/winamp/widgets/Telemetry.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>

#include <algorithm>
#include <functional>

namespace winamp {

namespace {

const QColor PingColors[] = {
    QColor(0x40, 0xa0, 0xff),
    QColor(0xff, 0x90, 0x30),
    QColor(0xff, 0x48, 0x38),
    QColor(0x38, 0xe0, 0xb0),
    QColor(0xff, 0xd8, 0x30),
};
constexpr int ColorCount = int(std::size(PingColors));

/// A row of indicator LEDs; the lit one is the ping colour.
class LedPicker : public QWidget {
public:
    std::function<void(int)> onPick;
    int selected = 0;

    LedPicker()
    {
        setFixedSize(ColorCount * 22 + 8, 26);
        setCursor(Qt::PointingHandCursor);
        setToolTip(tr("Ping colour"));
    }

protected:
    QPointF centre(int i) const { return QPointF(15 + i * 22, height() / 2.0); }

    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        for (int i = 0; i < ColorCount; ++i) {
            Metal::led(p, centre(i), 5, PingColors[i], i == selected);
        }
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        for (int i = 0; i < ColorCount; ++i) {
            if (QLineF(centre(i), e->position()).length() <= 10 && onPick) {
                onPick(i);
            }
        }
    }
};

/// The ping icon glowing inside a small recessed LCD well.
class IconWell : public QWidget {
public:
    explicit IconWell(Icon icon)
        : m_icon(icon)
    {
        setFixedSize(46, 34);
    }
    void setColor(const QColor& color)
    {
        m_color = color;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        Metal::lcd(p, QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), Theme::radiusPanel);
        Icons::paint(p, m_icon, QRectF(rect()).adjusted(12, 6, -12, -6), m_color);
    }

private:
    Icon m_icon;
    QColor m_color;
};

struct Ping {
    QString name;
    Icon icon;
    int color;
    bool visible;
};

class PingRow : public QWidget {
public:
    explicit PingRow(const Ping& ping)
        : m_ping(ping)
    {
        m_toggle = new CapsuleButton(tr("OFF")); // sized for the wider label
        m_well = new IconWell(ping.icon);
        m_name = new QLabel(ping.name);
        m_name->setFont(Metal::uiFont(10, true));
        m_leds = new LedPicker;

        auto* row = new QHBoxLayout(this);
        row->setContentsMargins(10, 8, 10, 8);
        row->setSpacing(16);
        row->addWidget(m_toggle);
        row->addWidget(m_well);
        row->addWidget(m_name, 1);
        row->addWidget(m_leds);

        connect(m_toggle, &CapsuleButton::clicked, this, [this] { setVisibleOnHud(!m_ping.visible); });
        m_leds->onPick = [this](int i) {
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
        Telemetry::report(QStringLiteral("%1: %2").arg(m_ping.name, visible ? tr("on") : tr("off")));
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this); // etched separator under each row
        p.fillRect(QRectF(6, height() - 2, width() - 12, 1), Theme::bevelDark);
        p.fillRect(QRectF(6, height() - 1, width() - 12, 1), Theme::bevelLight);
    }

private:
    void refresh()
    {
        m_toggle->setText(m_ping.visible ? tr("ON") : tr("OFF"));
        m_toggle->setActive(m_ping.visible);
        m_well->setColor(m_ping.visible ? PingColors[m_ping.color] : Theme::lcdDim.lighter(150));
        m_name->setStyleSheet(m_ping.visible ? QString() : QStringLiteral("color: #5a6478;"));
        m_leds->selected = m_ping.color;
        m_leds->update();
    }

    Ping m_ping;
    CapsuleButton* m_toggle;
    IconWell* m_well;
    QLabel* m_name;
    LedPicker* m_leds;
};

} // namespace

PingPage::PingPage(QWidget* parent)
    : QWidget(parent)
{
    setStatusTip(tr("3 of 4 pings visible"));
    const Ping pings[] = {
        { tr("Lifepod"), Icon::Lifepod, 0, true },
        { tr("Seamoth - SEAMOTH"), Icon::Seamoth, 3, true },
        { tr("Signal"), Icon::Signal, 2, true },
        { tr("Beacon - Kelp forest"), Icon::Beacon, 4, false },
    };

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setSpacing(2);

    auto* toggleAll = new CapsuleButton(tr("ALL OFF"));
    auto* header = new QHBoxLayout;
    auto* title = new QLabel(tr("PING MANAGER"));
    title->setFont(Metal::uiFont(9, true));
    header->addWidget(title);
    header->addStretch();
    header->addWidget(toggleAll);
    layout->addLayout(header);

    QList<PingRow*> rows;
    for (const Ping& ping : pings) {
        rows.append(new PingRow(ping));
        layout->addWidget(rows.last());
    }
    layout->addStretch();

    connect(toggleAll, &CapsuleButton::clicked, this, [rows, toggleAll] {
        const bool anyVisible = std::any_of(rows.begin(), rows.end(), [](PingRow* r) { return r->isVisibleOnHud(); });
        for (PingRow* r : rows) {
            r->setVisibleOnHud(!anyVisible);
        }
        toggleAll->setText(anyVisible ? tr("ALL ON") : tr("ALL OFF"));
    });
}

} // namespace winamp
