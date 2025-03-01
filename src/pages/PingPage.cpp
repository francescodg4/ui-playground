#include "PingPage.hpp"

#include "Icons.hpp"
#include "Theme.hpp"
#include "widgets/Holo.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>
#include <QToolButton>

#include <functional>

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
    static constexpr qreal Dot = 11, Gap = 10;
    std::function<void(int)> onPick;
    int selected = 0;

    ColorDots()
    {
        setFixedSize(int(ColorCount * Dot + (ColorCount - 1) * Gap + 8), 24);
        setCursor(Qt::PointingHandCursor);
        setToolTip(tr("Ping colour"));
    }

protected:
    QPointF centre(int i) const { return QPointF(4 + Dot / 2 + i * (Dot + Gap), height() / 2.0); }

    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        for (int i = 0; i < ColorCount; ++i) {
            p.setPen(Qt::NoPen);
            p.setBrush(PingColors[i]);
            p.drawEllipse(centre(i), Dot / 2, Dot / 2);
            if (i == selected) {
                p.setBrush(Qt::NoBrush);
                p.setPen(QPen(QColor(235, 248, 255, 230), 1.6));
                p.drawEllipse(centre(i), Dot / 2 + 3, Dot / 2 + 3);
            }
        }
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        for (int i = 0; i < ColorCount; ++i) {
            if (QLineF(centre(i), e->position()).length() <= Dot / 2 + Gap / 2 && onPick) {
                onPick(i);
            }
        }
    }
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
        m_eye = Holo::roundButton(Icon::Eye, QString());
        m_icon = new QLabel;
        m_icon->setFixedSize(40, 40);
        m_name = new QLabel(ping.name);
        m_dots = new ColorDots;

        auto* row = new QHBoxLayout(this);
        row->setContentsMargins(0, 0, 0, 0);
        row->setSpacing(26);
        row->addWidget(m_eye);
        row->addWidget(m_icon);
        row->addWidget(m_name, 1);
        row->addWidget(m_dots);

        connect(m_eye, &QToolButton::clicked, this, [this] { setVisibleOnHud(!m_ping.visible); });
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
        m_eye->setIcon(Icons::icon(m_ping.visible ? Icon::Eye : Icon::EyeOff));
        m_eye->setToolTip(m_ping.visible ? tr("Hide ping") : tr("Show ping"));
        m_name->setStyleSheet(m_ping.visible ? QStringLiteral("font-weight: bold;") : QStringLiteral("font-weight: bold; color: rgba(238, 248, 255, 90);"));
        m_dots->selected = m_ping.color;
        m_dots->update();
    }

    Ping m_ping;
    QToolButton* m_eye;
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
    list->setContentsMargins(20, 6, 40, 0);
    list->setSpacing(22);

    auto* toggleAll = Holo::roundButton(Icon::EyeOff, tr("Toggle all pings"));
    list->addWidget(toggleAll, 0, Qt::AlignLeft);
    list->addSpacing(8);

    QList<PingRow*> rows;
    for (const Ping& ping : pings) {
        rows.append(new PingRow(ping));
        list->addWidget(rows.last());
    }
    list->addStretch();

    connect(toggleAll, &QToolButton::clicked, this, [rows] {
        const bool anyVisible = std::any_of(rows.begin(), rows.end(), [](PingRow* r) { return r->isVisibleOnHud(); });
        for (PingRow* r : rows) {
            r->setVisibleOnHud(!anyVisible);
        }
    });

    auto* layout = Holo::pageLayout(this, tr("Ping Manager"));
    layout->addWidget(Holo::scrollArea(content), 1);
}
