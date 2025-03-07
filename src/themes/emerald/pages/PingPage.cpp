#include "PingPage.hpp"

#include "Icons.hpp"
#include "themes/emerald/Theme.hpp"
#include "themes/emerald/widgets/Pixel.hpp"
#include "themes/emerald/widgets/PixelWidgets.hpp"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QScrollArea>

#include <algorithm>
#include <functional>

namespace emerald {

using Pixel::Scale;

namespace {

// indexed palette of ping colours
const QColor PingColors[] = {
    QColor(0x40, 0xa8, 0xf8),
    QColor(0xf8, 0x90, 0x40),
    QColor(0xe0, 0x50, 0x38),
    QColor(0x48, 0xd0, 0xb0),
    QColor(0xf8, 0xd0, 0x38),
};
constexpr int ColorCount = int(std::size(PingColors));

/// Colour swatches; a bi-colour arrow and a highlighted tile mark the chosen one.
class Swatches : public QWidget {
public:
    static constexpr int Swatch = 10, Gap = 6;
    std::function<void(int)> onPick;
    int selected = 0;

    Swatches()
    {
        setFixedSize((ColorCount * (Swatch + Gap) + 4) * Scale, 22 * Scale);
        setCursor(Qt::PointingHandCursor);
        setToolTip(tr("Ping colour"));
    }

protected:
    QRect swatch(int i) const { return QRect(3 + i * (Swatch + Gap), 9, Swatch, Swatch); }

    void paintEvent(QPaintEvent*) override
    {
        Pixel::Canvas canvas(this);
        QPainter& p = canvas.p();
        for (int i = 0; i < ColorCount; ++i) {
            const QRect r = swatch(i);
            if (i == selected) {
                Pixel::highlightTile(p, r.adjusted(-2, -2, 2, 2));
                Pixel::arrow(p, QRect(r.center().x() - 3, 0, 7, 6), Pixel::Direction::Down);
            }
            Pixel::chamfer(p, r, PingColors[i], Theme::borderDark, 1);
            p.fillRect(r.left() + 2, r.top() + 1, r.width() - 4, 1, PingColors[i].lighter(140));
        }
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        for (int i = 0; i < ColorCount; ++i) {
            if (swatch(i).adjusted(-3, -9, 3, 3).contains(e->position().toPoint() / Scale) && onPick) {
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

class PingRow : public PaperBox {
public:
    explicit PingRow(const Ping& ping)
        : m_ping(ping)
    {
        m_key = new PixelKey(QString());
        m_sprite = new SpriteLabel(ping.icon, 20);
        m_name = new PixelLabel(ping.name, PixelLabel::Tone::Dark, true);
        m_swatches = new Swatches;

        auto* row = new QHBoxLayout(this);
        row->setContentsMargins(8 * Scale, 4 * Scale, 8 * Scale, 4 * Scale);
        row->setSpacing(10 * Scale);
        row->addWidget(m_key);
        row->addWidget(m_sprite);
        row->addWidget(m_name, 1);
        row->addWidget(m_swatches);

        connect(m_key, &PixelKey::clicked, this, [this] { setVisibleOnHud(!m_ping.visible); });
        m_swatches->onPick = [this](int i) {
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
        m_key->setLabel(m_ping.visible ? tr("HIDE") : tr("SHOW"));
        m_key->setFace(m_ping.visible ? Theme::kbdUpperBlue : Theme::keyGray);
        m_sprite->setAccent(m_ping.visible ? PingColors[m_ping.color] : Theme::keyGray);
        m_name->setTone(m_ping.visible ? PixelLabel::Tone::Dark : PixelLabel::Tone::Dim);
        m_swatches->selected = m_ping.color;
        m_swatches->update();
    }

    Ping m_ping;
    PixelKey* m_key;
    SpriteLabel* m_sprite;
    PixelLabel* m_name;
    Swatches* m_swatches;
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
    list->setContentsMargins(2 * Scale, 2 * Scale, 6 * Scale, 2 * Scale);
    list->setSpacing(4 * Scale);

    auto* toggleAll = new PixelKey(tr("HIDE ALL"), Theme::keyGray);
    list->addWidget(toggleAll, 0, Qt::AlignLeft);

    QList<PingRow*> rows;
    for (const Ping& ping : pings) {
        rows.append(new PingRow(ping));
        list->addWidget(rows.last());
    }
    list->addStretch();

    connect(toggleAll, &PixelKey::clicked, this, [rows, toggleAll] {
        const bool anyVisible = std::any_of(rows.begin(), rows.end(), [](PingRow* r) { return r->isVisibleOnHud(); });
        for (PingRow* r : rows) {
            r->setVisibleOnHud(!anyVisible);
        }
        toggleAll->setLabel(anyVisible ? tr("SHOW ALL") : tr("HIDE ALL"));
    });

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4 * Scale, 2 * Scale, 2 * Scale, 2 * Scale);
    layout->addWidget(PixelUi::scrollArea(content));
}

} // namespace emerald
