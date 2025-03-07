#include "BlueprintsPage.hpp"

#include "Icons.hpp"
#include "themes/emerald/Theme.hpp"
#include "themes/emerald/widgets/Pixel.hpp"
#include "themes/emerald/widgets/PixelWidgets.hpp"

#include <QGridLayout>
#include <QScrollArea>
#include <QVBoxLayout>

namespace emerald {

using Pixel::Scale;

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

/// Sprite on a tile with its name; the glove points at the hovered one.
class BlueprintTile : public QWidget {
public:
    static constexpr int Width = 80, Height = 66;

    explicit BlueprintTile(const Blueprint& bp)
        : m_bp(bp)
    {
        setToolTip(QString::fromLatin1(bp.name));
        setFixedSize(Width * Scale, Height * Scale);
        setAttribute(Qt::WA_Hover);
        setCursor(Qt::PointingHandCursor);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        Pixel::Canvas canvas(this);
        QPainter& p = canvas.p();
        const QRect tile(Width / 2 - 15, 11, 30, 28);
        if (underMouse()) {
            Pixel::highlightTile(p, tile);
        } else {
            Pixel::chamfer(p, tile, Theme::metalLight, Theme::metalDark, 2);
        }
        p.drawImage(tile.left() + 3, tile.top() + 2, Pixel::sprite(m_bp.icon, 24, m_bp.accent));
        if (m_bp.isNew) {
            const QRect tag(tile.right() - 8, tile.top() - 4, 14, 8);
            Pixel::chamfer(p, tag, Theme::badge, Theme::borderDark, 1);
            Pixel::text(p, tag.adjusted(1, -1, 0, 0), Qt::AlignCenter, QStringLiteral("N"), Theme::lightText, Theme::badge.darker(160));
        }
        Pixel::text(p, QRect(1, tile.bottom() + 2, Width - 3, 24), Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
            QString::fromLatin1(m_bp.name), Theme::text, Theme::textShadow);
        if (underMouse()) {
            Pixel::hand(p, QPoint(tile.center().x(), tile.top() + 1), Pixel::Direction::Down);
        }
    }

private:
    Blueprint m_bp;
};

} // namespace

BlueprintsPage::BlueprintsPage(QWidget* parent)
    : QWidget(parent)
{
    constexpr int Columns = 6;
    auto* content = new QWidget;
    auto* list = new QVBoxLayout(content);
    list->setContentsMargins(2 * Scale, 2 * Scale, 6 * Scale, 2 * Scale);
    list->setSpacing(4 * Scale);

    for (const Category& cat : categories()) {
        list->addWidget(new BannerStrip(QString::fromLatin1(cat.name).toUpper(), Pixel::Wallpaper::City));
        auto* grid = new QGridLayout;
        grid->setSpacing(0);
        for (int i = 0; i < cat.items.size(); ++i) {
            grid->addWidget(new BlueprintTile(cat.items[i]), i / Columns, i % Columns);
        }
        grid->setColumnStretch(Columns, 1);
        list->addLayout(grid);
    }
    list->addStretch();

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4 * Scale, 2 * Scale, 2 * Scale, 2 * Scale);
    layout->addWidget(PixelUi::scrollArea(content));
}

} // namespace emerald
