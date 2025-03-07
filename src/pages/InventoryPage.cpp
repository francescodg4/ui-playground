#include "InventoryPage.hpp"

#include "Icons.hpp"
#include "Theme.hpp"
#include "widgets/Metal.hpp"
#include "widgets/MetalWidgets.hpp"
#include "widgets/Telemetry.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include <utility>

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

/// 6×6 storage grid drawn on the LCD glass; the selected cell glows.
class InventoryGrid : public QWidget {
public:
    static constexpr int Cols = 6, Rows = 6, Cell = 48;

    InventoryGrid()
    {
        setMouseTracking(true);
        setFixedSize(Cols * Cell + 1, Rows * Cell + 1);
    }

protected:
    QRectF itemRect(const Item& it) const { return QRectF(it.col * Cell, it.row * Cell, it.w * Cell, it.h * Cell).adjusted(3, 3, -3, -3); }

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
        p.setPen(QPen(Theme::lcdDim, 1));
        for (int i = 0; i <= Cols; ++i) {
            p.drawLine(QPointF(i * Cell + 0.5, 0), QPointF(i * Cell + 0.5, Rows * Cell));
        }
        for (int i = 0; i <= Rows; ++i) {
            p.drawLine(QPointF(0, i * Cell + 0.5), QPointF(Cols * Cell, i * Cell + 0.5));
        }
        for (int i = 0; i < int(std::size(Inventory)); ++i) {
            const Item& it = Inventory[i];
            const QRectF r = itemRect(it);
            QPainterPath cell;
            cell.addRoundedRect(r, Theme::radiusPanel, Theme::radiusPanel);
            p.fillPath(cell, i == m_selected || i == m_hover ? QColor(0x3a, 0x62, 0xa0) : QColor(0x24, 0x44, 0x7a));
            if (i == m_selected) {
                QColor halo = Theme::lcdGlow;
                halo.setAlpha(90);
                p.strokePath(cell, QPen(halo, 4));
                p.strokePath(cell, QPen(Theme::lcdGlow, 1.2));
            }
            const qreal inset = std::min(r.width(), r.height()) * 0.14;
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
        if (m_selected >= 0) {
            Telemetry::report(tr("Selected: %1").arg(QString::fromLatin1(Inventory[m_selected].name)));
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
    { Icon::Mask, "Head", 0.50, 0.13 },
    { Icon::Compass, "Compass", 0.20, 0.32, true },
    { Icon::Chip, "Chip slot", 0.80, 0.32 },
    { Icon::Diver, "Diving suit", 0.50, 0.50, true, true },
    { Icon::Tank, "Tank", 0.20, 0.68 },
    { Icon::Gloves, "Gloves", 0.80, 0.68 },
    { Icon::Fins, "Fins", 0.50, 0.87, true },
};

/// Equipment as circular bevel-edged buttons; equipped ones carry the blue accent.
class EquipmentView : public QWidget {
public:
    EquipmentView()
    {
        setMinimumSize(280, 300);
        setMouseTracking(true);
    }

protected:
    qreal radius(const Slot& s) const { return std::min(width(), height()) * (s.body ? 0.16 : 0.10); }
    QPointF centre(const Slot& s) const { return QPointF(s.x * width(), s.y * height()); }

    int slotAt(const QPointF& pos) const
    {
        for (int i = 0; i < int(std::size(Slots)); ++i) {
            if (QLineF(centre(Slots[i]), pos).length() <= radius(Slots[i])) {
                return i;
            }
        }
        return -1;
    }

    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        for (int i = 0; i < int(std::size(Slots)); ++i) {
            const Slot& s = Slots[i];
            const qreal r = radius(s);
            const QRectF body(centre(s) - QPointF(r, r), QSizeF(2 * r, 2 * r));
            const bool pressed = i == m_pressed;
            Metal::roundButton(p, body, pressed, s.filled, i == m_hover);
            const qreal icon = r * (s.body ? 0.95 : 0.9);
            const QPointF shift = pressed ? QPointF(0.5, 1) : QPointF();
            Icons::paint(p, s.icon, QRectF(centre(s) - QPointF(icon / 2, icon / 2) + shift, QSizeF(icon, icon)),
                s.filled ? Theme::accent : Theme::textDim);
        }
    }

    void mouseMoveEvent(QMouseEvent* e) override
    {
        const int i = slotAt(e->position());
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
        m_pressed = slotAt(e->position());
        update();
    }

    void mouseReleaseEvent(QMouseEvent* e) override
    {
        const int pressed = std::exchange(m_pressed, -1);
        if (pressed >= 0 && pressed == slotAt(e->position())) {
            const Slot& s = Slots[pressed];
            Telemetry::report(QString::fromLatin1(s.name) + (s.filled ? tr(": equipped") : tr(": empty")));
        }
        update();
    }

private:
    int m_hover = -1;
    int m_pressed = -1;
};

} // namespace

InventoryPage::InventoryPage(QWidget* parent)
    : QWidget(parent)
{
    setStatusTip(tr("8 items stored - 3 equipped"));

    auto* gridPanel = new LcdPanel;
    auto* gridLayout = new QVBoxLayout(gridPanel);
    gridLayout->setContentsMargins(12, 8, 12, 12);
    gridLayout->addWidget(MetalUi::lcdLabel(tr("INVENTORY  8/36")));
    gridLayout->addWidget(new InventoryGrid, 0, Qt::AlignCenter);
    gridLayout->addStretch();

    auto* title = new QLabel(tr("EQUIPPED"));
    title->setFont(Metal::uiFont(9, true));
    auto* right = new QVBoxLayout;
    right->addWidget(title);
    right->addWidget(new EquipmentView, 1);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(14);
    layout->addWidget(gridPanel, 0);
    layout->addLayout(right, 1);
}
