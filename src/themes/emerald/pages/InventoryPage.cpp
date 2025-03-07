#include "InventoryPage.hpp"

#include "Icons.hpp"
#include "themes/emerald/Theme.hpp"
#include "themes/emerald/widgets/Pixel.hpp"
#include "themes/emerald/widgets/PixelWidgets.hpp"

#include <QHBoxLayout>
#include <QMouseEvent>

#include <functional>

namespace emerald {

using Pixel::Scale;

namespace {

struct Item {
    Icon icon;
    const char* name;
    int col, row, w = 1, h = 1;
};

const Item Inventory[] = {
    { Icon::Fabricator, "Mobile fabricator", 0, 0, 2, 2 },
    { Icon::Flashlight, "Flashlight", 2, 0, 1, 2 },
    { Icon::Titanium, "Titanium", 3, 0 },
    { Icon::Titanium, "Titanium", 3, 1 },
    { Icon::Battery, "Battery", 4, 0 },
    { Icon::Repair, "Repair tool", 0, 2, 2, 1 },
    { Icon::Titanium, "Titanium", 2, 2 },
    { Icon::Knife, "Survival knife", 3, 2 },
};

/// 6×5 storage box on the wallpaper: the glove follows the pointer, the chosen item sits on
/// a highlighted tile.
class BoxGrid : public QWidget {
public:
    static constexpr int Cols = 6, Rows = 5, Cell = 32, Top = 14;
    std::function<void(const QString&)> onSelect;

    BoxGrid()
    {
        setMouseTracking(true);
        setFixedSize((Cols * Cell + 2) * Scale, (Top + Rows * Cell + 2) * Scale);
    }

protected:
    QRect itemRect(const Item& it) const { return QRect(1 + it.col * Cell, Top + it.row * Cell, it.w * Cell, it.h * Cell); }

    int itemAt(const QPoint& devicePos) const
    {
        for (int i = 0; i < int(std::size(Inventory)); ++i) {
            if (itemRect(Inventory[i]).contains(devicePos / Scale)) {
                return i;
            }
        }
        return -1;
    }

    void paintEvent(QPaintEvent*) override
    {
        Pixel::Canvas canvas(this);
        QPainter& p = canvas.p();
        for (int i = 0; i < int(std::size(Inventory)); ++i) {
            const Item& it = Inventory[i];
            const QRect r = itemRect(it);
            if (i == m_selected) {
                Pixel::highlightTile(p, r.adjusted(1, 1, -1, -1));
            }
            const int s = std::min(r.width(), r.height()) - 4;
            p.drawImage(r.center().x() - s / 2 + 1, r.center().y() - s / 2 + 1, Pixel::sprite(it.icon, s));
        }
        const int focus = m_hover >= 0 ? m_hover : m_selected;
        if (focus >= 0) {
            const QRect r = itemRect(Inventory[focus]);
            Pixel::hand(p, QPoint(r.center().x(), r.top() + 3), Pixel::Direction::Down);
        }
    }

    void mouseMoveEvent(QMouseEvent* e) override
    {
        const int i = itemAt(e->position().toPoint());
        setCursor(i >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
        if (i != m_hover) {
            m_hover = i;
            update();
        }
    }

    void leaveEvent(QEvent*) override
    {
        m_hover = -1;
        update();
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        m_selected = itemAt(e->position().toPoint());
        if (onSelect) {
            onSelect(m_selected >= 0 ? QString::fromLatin1(Inventory[m_selected].name) : QString());
        }
        update();
    }

private:
    int m_hover = -1;
    int m_selected = -1;
};

struct Slot {
    Icon icon;
    const char* name;
    bool filled;
};

const Slot Slots[] = {
    { Icon::Mask, "Mask", false },
    { Icon::Diver, "Diving suit", true },
    { Icon::Compass, "Compass", true },
    { Icon::Tank, "Tank", false },
    { Icon::Gloves, "Gloves", false },
    { Icon::Fins, "Fins", true },
};

/// Teal party deck: one stacked slot per piece of equipment.
class EquipDeck : public QWidget {
public:
    static constexpr int Width = 132, SlotHeight = 22, Top = 18;
    std::function<void(const QString&)> onSelect;

    EquipDeck()
    {
        setFixedSize(Width * Scale, (Top + int(std::size(Slots)) * (SlotHeight + 3) + 6) * Scale);
        setCursor(Qt::PointingHandCursor);
    }

protected:
    QRect slotRect(int i) const { return QRect(6, Top + i * (SlotHeight + 3), Width - 12, SlotHeight); }

    void paintEvent(QPaintEvent*) override
    {
        Pixel::Canvas canvas(this);
        QPainter& p = canvas.p();
        Pixel::deck(p, canvas.rect());
        p.setFont(Pixel::font(true));
        Pixel::text(p, QRect(8, 3, Width - 16, 12), Qt::AlignLeft | Qt::AlignVCenter, tr("EQUIPPED"), Theme::lightText, Theme::darkGreen);
        p.setFont(Pixel::font());
        for (int i = 0; i < int(std::size(Slots)); ++i) {
            const Slot& s = Slots[i];
            const QRect r = slotRect(i);
            const bool selected = i == m_selected;
            Pixel::deckSlot(p, r, s.filled, selected);
            const QImage sprite = Pixel::sprite(s.icon, 16, s.filled ? Qt::white : Theme::lcdBlue);
            p.drawImage(r.left() + 3, r.top() + 3, sprite);
            const QColor fg = selected ? Theme::text : s.filled ? Theme::lightText : Theme::kbdUpperBlue;
            const QColor shadow = selected ? Theme::textShadow : Theme::darkGreen.darker(150);
            Pixel::text(p, r.adjusted(23, 0, -2, 0), Qt::AlignLeft | Qt::AlignVCenter,
                s.filled ? QString::fromLatin1(s.name) : QStringLiteral("- - - - -"), fg, shadow);
        }
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        for (int i = 0; i < int(std::size(Slots)); ++i) {
            if (slotRect(i).contains(e->position().toPoint() / Scale)) {
                m_selected = i;
                if (onSelect) {
                    onSelect(QString::fromLatin1(Slots[i].name) + (Slots[i].filled ? QString() : tr(" (empty)")));
                }
                update();
            }
        }
    }

private:
    int m_selected = -1;
};

} // namespace

InventoryPage::InventoryPage(QWidget* parent)
    : QWidget(parent)
{
    auto* grid = new BoxGrid;
    auto* deck = new EquipDeck;

    auto* hintBox = new PaperBox;
    auto* hint = new PixelLabel(tr("Choose an item."));
    auto* hintLayout = new QHBoxLayout(hintBox);
    hintLayout->setContentsMargins(8 * Scale, 5 * Scale, 8 * Scale, 5 * Scale);
    hintLayout->addWidget(hint);
    grid->onSelect = [hint](const QString& name) { hint->setText(name.isEmpty() ? QObject::tr("Choose an item.") : name); };
    deck->onSelect = grid->onSelect;

    auto* columns = new QHBoxLayout;
    columns->setSpacing(8 * Scale);
    columns->addStretch();
    columns->addWidget(grid, 0, Qt::AlignTop);
    columns->addStretch();
    columns->addWidget(deck, 0, Qt::AlignTop);
    columns->addStretch();

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4 * Scale, 2 * Scale, 4 * Scale, 4 * Scale);
    layout->setSpacing(6 * Scale);
    layout->addLayout(columns, 1);
    layout->addWidget(hintBox);
}

} // namespace emerald
