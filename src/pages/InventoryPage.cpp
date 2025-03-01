#include "InventoryPage.hpp"

#include "Icons.hpp"
#include "Theme.hpp"
#include "widgets/Holo.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include <functional>

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

        p.setPen(QPen(QColor(185, 230, 255, 40), 1));
        for (int r = 0; r < Rows; ++r) {
            for (int c = 0; c < Cols; ++c) {
                p.drawRect(QRectF(Pad + c * Cell, Pad + r * Cell, Cell, Cell));
            }
        }

        // corner brackets
        const QRectF frame = QRectF(rect()).adjusted(1, 1, -1, -1);
        const qreal L = 12;
        p.setPen(QPen(QColor(215, 240, 255, 180), 2));
        for (const QPointF corner : { frame.topLeft(), frame.topRight(), frame.bottomLeft(), frame.bottomRight() }) {
            const qreal sx = corner.x() < width() / 2 ? 1 : -1;
            const qreal sy = corner.y() < height() / 2 ? 1 : -1;
            p.drawLine(corner, corner + QPointF(sx * L, 0));
            p.drawLine(corner, corner + QPointF(0, sy * L));
        }

        for (int i = 0; i < int(std::size(Inventory)); ++i) {
            const Item& it = Inventory[i];
            const QRectF r = itemRect(it);
            const bool lit = i == m_hover || i == m_selected;
            QPainterPath shape;
            if (it.small) {
                const qreal d = std::min(r.width(), r.height()) - 8;
                shape.addEllipse(r.center(), d / 2, d / 2);
                p.fillPath(shape, QColor(160, 205, 235, 90));
            } else {
                shape.addRoundedRect(r, 10, 10);
                QRadialGradient g(r.center() - QPointF(0, r.height() * 0.1), std::max(r.width(), r.height()) * 0.7);
                g.setColorAt(0, QColor(150, 215, 255, 120));
                g.setColorAt(1, QColor(40, 120, 200, 90));
                p.fillPath(shape, g);
            }
            if (lit) {
                p.strokePath(shape, QPen(QColor(140, 220, 255, 120), 6));
            }
            p.strokePath(shape, QPen(QColor(215, 240, 255, it.small ? 90 : 180), 1.5));
            const qreal inset = it.small ? r.width() * 0.26 : std::min(r.width(), r.height()) * 0.12;
            Icons::paint(p, it.icon, r.adjusted(inset, inset, -inset, -inset));
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
        m_selected = itemAt(e->position());
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
        for (const Slot& s : Slots) {
            const qreal r = radius(s);
            const QPointF c = centre(s);
            if (s.filled) {
                QRadialGradient g(c - QPointF(0, r * 0.2), r * 1.1);
                g.setColorAt(0, QColor(160, 220, 255, 140));
                g.setColorAt(1, QColor(40, 120, 200, 115));
                p.setBrush(g);
                p.setPen(QPen(QColor(225, 245, 255, 215), 1.5));
            } else {
                p.setBrush(QColor(150, 200, 235, 46));
                p.setPen(QPen(QColor(215, 240, 255, 115), 1.5));
            }
            p.drawEllipse(c, r, r);
            const qreal s2 = r * (s.body ? 0.8 : 0.62);
            Icons::paint(p, s.icon, QRectF(c.x() - s2, c.y() - s2, 2 * s2, 2 * s2), s.filled ? Qt::white : QColor(225, 240, 255, 115));
        }
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        for (const Slot& s : Slots) {
            if (QLineF(centre(s), e->position()).length() <= radius(s)) {
                if (onSelect) {
                    onSelect(QString::fromLatin1(s.name) + (s.filled ? QString() : tr(" (empty)")));
                }
                return;
            }
        }
    }
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

    auto* left = new QVBoxLayout;
    left->addWidget(new SectionTitle(tr("Inventory")), 0, Qt::AlignHCenter);
    left->addSpacing(8);
    left->addWidget(grid, 0, Qt::AlignHCenter);
    left->addStretch();

    auto* right = new QVBoxLayout;
    right->addWidget(new SectionTitle(tr("Equipped")), 0, Qt::AlignHCenter);
    right->addWidget(equipment, 1);

    auto* columns = new QHBoxLayout;
    columns->setSpacing(30);
    columns->addLayout(left, 1);
    columns->addLayout(right, 1);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(columns, 1);
    layout->addWidget(hint);
}
