#include "InventoryPage.hpp"

#include "Icons.hpp"
#include "themes/glass/Theme.hpp"
#include "themes/glass/widgets/Glass.hpp"
#include "themes/glass/widgets/Holo.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include <functional>
#include <utility>

namespace glass {

namespace {

struct Item {
    Icon icon;
    const char* name;
    int col, row, w = 1, h = 1;
    bool small = false; ///< drawn as a round token instead of a card
};

const Item Inventory[] = {
    { Icon::Fabricator, "Mobile fabricator", 0, 0, 2, 2 },
    { Icon::Flashlight, "Flashlight", 2, 0, 1, 2 },
    { Icon::Titanium, "Titanium", 3, 0, 1, 1, true },
    { Icon::Titanium, "Titanium", 3, 1, 1, 1, true },
    { Icon::Battery, "Battery", 4, 0, 1, 1, true },
    { Icon::Repair, "Repair tool", 0, 2, 2, 1 },
    { Icon::Titanium, "Titanium", 2, 2, 1, 1, true },
    { Icon::Knife, "Survival knife", 3, 2, 1, 1, true },
};

/// 6×6 storage grid with corner brackets; items span one or more cells.
class InventoryGrid : public QWidget {
public:
    static constexpr int Cols = 6, Rows = 6, Cell = 52, Pad = 8;
    std::function<void(const QString&)> onSelect;

    InventoryGrid()
    {
        setMouseTracking(true);
        setFixedSize(Cols * Cell + 2 * Pad, Rows * Cell + 2 * Pad);
    }

protected:
    QRectF itemRect(const Item& it) const
    {
        return QRectF(Pad + it.col * Cell, Pad + it.row * Cell, it.w * Cell, it.h * Cell).adjusted(3, 3, -3, -3);
    }

    int itemAt(const QPointF& pos) const
    {
        for (int i = 0; i < int(std::size(Inventory)); ++i) {
            if (itemRect(Inventory[i]).contains(pos)) {
                return i;
            }
        }
        return -1;
    }

    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // low-contrast bento cells
        p.setPen(QPen(Theme::glassBorder, 1));
        for (int r = 0; r < Rows; ++r) {
            for (int c = 0; c < Cols; ++c) {
                p.drawRoundedRect(QRectF(Pad + c * Cell, Pad + r * Cell, Cell, Cell).adjusted(2, 2, -2, -2), 8, 8);
            }
        }

        for (int i = 0; i < int(std::size(Inventory)); ++i) {
            const Item& it = Inventory[i];
            QRectF r = itemRect(it);
            if (it.small) {
                const qreal d = std::min(r.width(), r.height()) - 8;
                r = QRectF(r.center().x() - d / 2, r.center().y() - d / 2, d, d);
            }
            const qreal radius = it.small ? r.height() / 2 : Theme::radiusControl;
            if (i == m_selected) {
                Glass::paintGlow(p, r, radius, Theme::glowAccent);
            } else if (i == m_hover) {
                Glass::paintGlow(p, r, radius, Theme::glowSoft, 0.7);
            }
            Glass::paintClay(p, r, radius, i == m_selected ? Theme::accent : Theme::clay, i == m_pressed);
            const QRectF face = i == m_pressed ? Glass::scaled(r, Theme::pressedScale) : r;
            const qreal inset = it.small ? face.width() * 0.2 : std::min(face.width(), face.height()) * 0.12;
            Icons::paint(p, it.icon, face.adjusted(inset, inset, -inset, -inset));
        }
    }

    void mouseMoveEvent(QMouseEvent* e) override
    {
        const int i = itemAt(e->position());
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
        m_pressed = itemAt(e->position());
        update();
    }

    void mouseReleaseEvent(QMouseEvent* e) override
    {
        const int pressed = std::exchange(m_pressed, -1);
        if (pressed == itemAt(e->position())) {
            m_selected = pressed;
            if (onSelect) {
                onSelect(m_selected >= 0 ? QString::fromLatin1(Inventory[m_selected].name) : QString());
            }
        }
        update();
    }

private:
    int m_hover = -1;
    int m_pressed = -1;
    int m_selected = -1;
};

struct Slot {
    Icon icon;
    const char* name;
    qreal x, y; ///< centre, as a fraction of the view
    bool filled = false;
    bool body = false;
};

const Slot Slots[] = {
    { Icon::Mask, "Head", 0.50, 0.14 },
    { Icon::Compass, "Compass", 0.22, 0.32, true },
    { Icon::Chip, "Chip slot", 0.78, 0.32 },
    { Icon::Diver, "Diving suit", 0.50, 0.50, true, true },
    { Icon::Tank, "Tank", 0.22, 0.68 },
    { Icon::Gloves, "Gloves", 0.78, 0.68 },
    { Icon::Fins, "Fins", 0.50, 0.86, true },
};

/// Equipment slots arranged around the diver.
class EquipmentView : public QWidget {
public:
    std::function<void(const QString&)> onSelect;

    EquipmentView() { setMinimumSize(300, 300); }

protected:
    qreal radius(const Slot& s) const
    {
        const qreal unit = std::min(width(), height());
        return s.body ? unit * 0.16 : unit * 0.105;
    }
    QPointF centre(const Slot& s) const { return QPointF(s.x * width(), s.y * height()); }

    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        for (int i = 0; i < int(std::size(Slots)); ++i) {
            const Slot& s = Slots[i];
            const qreal r = radius(s);
            const QRectF body(centre(s) - QPointF(r, r), QSizeF(2 * r, 2 * r));
            const bool pressed = i == m_pressed;
            if (s.body) {
                Glass::paintGlow(p, body, r, Theme::glowAccent, 0.8);
            }
            // filled slots are raised clay; empty ones sit recessed and flat
            Glass::paintClay(p, body, r, s.filled ? Theme::clay : Theme::clayRecessed, pressed || !s.filled);
            const QRectF face = pressed ? Glass::scaled(body, Theme::pressedScale) : body;
            const qreal s2 = face.width() / 2 * (s.body ? 0.8 : 0.62);
            Icons::paint(p, s.icon, QRectF(face.center() - QPointF(s2, s2), QSizeF(2 * s2, 2 * s2)), s.filled ? Qt::white : Theme::iconMuted);
        }
    }

    int slotAt(const QPointF& pos) const
    {
        for (int i = 0; i < int(std::size(Slots)); ++i) {
            if (QLineF(centre(Slots[i]), pos).length() <= radius(Slots[i])) {
                return i;
            }
        }
        return -1;
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        m_pressed = slotAt(e->position());
        update();
    }

    void mouseReleaseEvent(QMouseEvent* e) override
    {
        const int pressed = std::exchange(m_pressed, -1);
        if (pressed >= 0 && pressed == slotAt(e->position()) && onSelect) {
            const Slot& s = Slots[pressed];
            onSelect(QString::fromLatin1(s.name) + (s.filled ? QString() : tr(" (empty)")));
        }
        update();
    }

private:
    int m_pressed = -1;
};

} // namespace

InventoryPage::InventoryPage(QWidget* parent)
    : QWidget(parent)
{
    auto* grid = new InventoryGrid;
    auto* equipment = new EquipmentView;
    auto* hint = new QLabel(QStringLiteral(" "));
    hint->setObjectName(QStringLiteral("dim"));
    hint->setAlignment(Qt::AlignCenter);
    grid->onSelect = [hint](const QString& name) { hint->setText(name.isEmpty() ? QStringLiteral(" ") : name); };
    equipment->onSelect = grid->onSelect;

    auto* gridCell = new QVBoxLayout;
    gridCell->addWidget(grid, 0, Qt::AlignCenter);
    auto* left = new QVBoxLayout;
    left->setSpacing(Theme::gap - 4);
    left->addWidget(new SectionTitle(tr("Inventory")), 0, Qt::AlignHCenter);
    left->addWidget(Holo::card(gridCell), 1);

    auto* equipmentCell = new QVBoxLayout;
    equipmentCell->addWidget(equipment);
    auto* right = new QVBoxLayout;
    right->setSpacing(Theme::gap - 4);
    right->addWidget(new SectionTitle(tr("Equipped")), 0, Qt::AlignHCenter);
    right->addWidget(Holo::card(equipmentCell), 1);

    auto* columns = new QHBoxLayout;
    columns->setSpacing(Theme::gap);
    columns->addLayout(left, 1);
    columns->addLayout(right, 1);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(columns, 1);
    layout->addWidget(hint);
}

} // namespace glass
