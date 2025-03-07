#include "BlueprintsPage.hpp"

#include "Icons.hpp"
#include "Theme.hpp"
#include "widgets/Metal.hpp"
#include "widgets/MetalWidgets.hpp"
#include "widgets/Telemetry.hpp"

#include <QListWidget>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

namespace {

struct Blueprint {
    Icon icon;
    const char* name;
    QColor accent = Qt::white;
    bool isNew = false;
};

struct Category {
    const char* name;
    QList<Blueprint> items;
};

const QList<Category>& categories()
{
    static const QList<Category> list = {
        { "Basic Materials",
            {
                { Icon::Titanium, "Titanium" },
                { Icon::Ingot, "Titanium ingot" },
                { Icon::Mesh, "Fiber mesh" },
                { Icon::Mesh, "Silicone rubber" },
                { Icon::Glass, "Glass" },
                { Icon::Bottle, "Bleach", QColor(0xd8, 0x3a, 0x2a) },
                { Icon::Drop, "Lubricant" },
                { Icon::Glass, "Enameled glass" },
                { Icon::Ingot, "Plasteel ingot", Qt::white, true },
            } },
        { "Advanced Materials",
            {
                { Icon::Bottle, "Hydrochloric acid", QColor(0xe8, 0xb0, 0x3a) },
                { Icon::Bottle, "Benzene", QColor(0xd8, 0x4a, 0x3a) },
                { Icon::Fiber, "Synthetic fibers" },
                { Icon::Aerogel, "Aerogel" },
                { Icon::Bottle, "Polyaniline", QColor(0x5b, 0xd6, 0x6a) },
                { Icon::Bottle, "Hatching enzymes", QColor(0xb9, 0xe4, 0xff) },
            } },
        { "Electronics",
            {
                { Icon::Battery, "Battery" },
                { Icon::Battery, "Power cell" },
                { Icon::Scanner, "Computer chip" },
            } },
        { "Tools",
            {
                { Icon::Scanner, "Scanner" },
                { Icon::Repair, "Repair tool" },
                { Icon::Flashlight, "Flashlight" },
                { Icon::Knife, "Survival knife" },
            } },
    };
    return list;
}

enum Role { HeaderRole = Qt::UserRole, IconRole, AccentRole, NumberRole, CategoryRole, NewRole };

/// Playlist rows on the LCD: numbered entries, category headers, glowing monospace text.
class PlaylistDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex& index) const override
    {
        return QSize(200, index.data(HeaderRole).toBool() ? 26 : 22);
    }

    void paint(QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        const QRectF r = QRectF(option.rect).adjusted(2, 1, -2, -1);
        if (index.data(HeaderRole).toBool()) {
            p->setFont(Metal::digitalFont(12, true));
            Metal::glowText(*p, r.adjusted(4, 0, 0, -2), Qt::AlignLeft | Qt::AlignBottom, index.data().toString().toUpper(), Theme::lcdGlow);
            p->fillRect(QRectF(r.left() + 4, r.bottom(), r.width() - 8, 1), Theme::lcdDim);
            p->restore();
            return;
        }
        const bool selected = option.state & QStyle::State_Selected;
        if (selected || (option.state & QStyle::State_MouseOver)) {
            p->fillRect(r, selected ? Theme::accent : Theme::lcdRow);
        }
        const QColor ink = selected ? Qt::white : Theme::lcdGlow;
        p->setFont(Metal::digitalFont(12));
        const QString number = QStringLiteral("%1.").arg(index.data(NumberRole).toInt(), 2, 10, QLatin1Char('0'));
        p->setPen(ink);
        p->drawText(r.adjusted(6, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, number);
        const QRectF chip(r.left() + 36, r.center().y() - 9, 20, 18);
        p->setPen(Qt::NoPen);
        p->setBrush(QColor(0x3a, 0x62, 0xa0));
        p->drawRoundedRect(chip, 4, 4);
        Icons::paint(*p, Icon(index.data(IconRole).toInt()), chip.adjusted(2, 1, -2, -1), index.data(AccentRole).value<QColor>());
        Metal::glowText(*p, r.adjusted(62, 0, -160, 0), Qt::AlignVCenter | Qt::AlignLeft, index.data().toString(), ink);
        if (index.data(NewRole).toBool()) {
            const QRectF badge(r.right() - 196, r.center().y() - 7, 34, 14);
            p->setPen(Qt::NoPen);
            p->setBrush(Theme::lcdGlow);
            p->drawRoundedRect(badge, 3, 3);
            p->setPen(Theme::lcdBg);
            p->setFont(Metal::digitalFont(10, true));
            p->drawText(badge, Qt::AlignCenter, QStringLiteral("NEW"));
            p->setFont(Metal::digitalFont(12));
        }
        p->setPen(selected ? Qt::white : Theme::lcdDim.lighter(200));
        p->drawText(r.adjusted(0, 0, -8, 0), Qt::AlignVCenter | Qt::AlignRight, index.data(CategoryRole).toString().toUpper());
        p->restore();
    }
};

} // namespace

BlueprintsPage::BlueprintsPage(QWidget* parent)
    : QWidget(parent)
{
    auto* list = new QListWidget;
    list->setItemDelegate(new PlaylistDelegate(list));
    list->setMouseTracking(true);
    list->setFocusPolicy(Qt::NoFocus);
    list->viewport()->setAutoFillBackground(false);
    list->setCursor(Qt::PointingHandCursor);

    int number = 0;
    for (const Category& cat : categories()) {
        auto* header = new QListWidgetItem(QString::fromLatin1(cat.name), list);
        header->setData(HeaderRole, true);
        header->setFlags(Qt::NoItemFlags);
        for (const Blueprint& bp : cat.items) {
            auto* item = new QListWidgetItem(QString::fromLatin1(bp.name), list);
            item->setData(IconRole, int(bp.icon));
            item->setData(AccentRole, bp.accent);
            item->setData(NumberRole, ++number);
            item->setData(CategoryRole, QString::fromLatin1(cat.name));
            item->setData(NewRole, bp.isNew);
        }
    }
    setStatusTip(tr("%1 blueprints - 1 new").arg(number));
    connect(list, &QListWidget::itemClicked, this, [](QListWidgetItem* item) {
        Telemetry::report(tr("Blueprint: %1 (%2)").arg(item->text(), item->data(CategoryRole).toString()));
    });

    auto* panel = new LcdPanel;
    auto* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(8, 6, 6, 8);
    panelLayout->addWidget(list);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(panel);
}
