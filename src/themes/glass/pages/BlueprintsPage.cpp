#include "BlueprintsPage.hpp"

#include "Icons.hpp"
#include "themes/glass/Theme.hpp"
#include "themes/glass/widgets/Glass.hpp"
#include "themes/glass/widgets/Holo.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QVBoxLayout>

namespace glass {

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

/// Round token with the item icon and its name underneath.
class BlueprintTile : public QWidget {
public:
    explicit BlueprintTile(const Blueprint& bp)
        : m_bp(bp)
    {
        setToolTip(QString::fromLatin1(bp.name));
        setMinimumSize(96, 108);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setAttribute(Qt::WA_Hover);
        setCursor(Qt::PointingHandCursor);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QRectF disc(width() / 2.0 - 29, 6, 58, 58);
        if (underMouse()) {
            Glass::paintGlow(p, disc, 29, Theme::glowSoft, 0.8);
        }
        Glass::paintClay(p, disc, 29, Theme::clay, m_pressed);
        const QRectF face = m_pressed ? Glass::scaled(disc, Theme::pressedScale) : disc;
        Icons::paint(p, m_bp.icon, face.adjusted(11, 11, -11, -11), m_bp.accent);

        if (m_bp.isNew) {
            p.save();
            const QRectF b(disc.right() - 14, disc.top() - 4, 18, 18);
            Glass::paintGlow(p, b, 9, Theme::glowBadge);
            Glass::paintClay(p, b, 9, Theme::clayBadge);
            QFont f = font();
            f.setBold(true);
            f.setPixelSize(10);
            p.setFont(f);
            p.setPen(QColor(0x3b, 0x24, 0x00));
            p.drawText(b, Qt::AlignCenter, QStringLiteral("!"));
            p.restore();
        }

        p.setPen(Theme::text);
        p.drawText(QRectF(0, 6 + 58 + 8, width(), height() - 58 - 14),
            Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, QString::fromLatin1(m_bp.name));
    }

    void mousePressEvent(QMouseEvent*) override
    {
        m_pressed = true;
        update();
    }

    void mouseReleaseEvent(QMouseEvent*) override
    {
        m_pressed = false;
        update();
    }

private:
    Blueprint m_bp;
    bool m_pressed = false;
};

} // namespace

BlueprintsPage::BlueprintsPage(QWidget* parent)
    : QWidget(parent)
{
    constexpr int Columns = 6;
    auto* content = new QWidget;
    auto* list = new QVBoxLayout(content);
    list->setContentsMargins(0, 0, 12, 0);
    list->setSpacing(Theme::gap);

    for (const Category& cat : categories()) {
        auto* header = new QLabel(QString::fromLatin1(cat.name));
        header->setObjectName(QStringLiteral("cardTitle"));

        auto* grid = new QGridLayout;
        grid->setHorizontalSpacing(6);
        grid->setVerticalSpacing(10);
        grid->addWidget(header, 0, 0, 1, Columns);
        for (int i = 0; i < cat.items.size(); ++i) {
            grid->addWidget(new BlueprintTile(cat.items[i]), 1 + i / Columns, i % Columns);
        }
        for (int c = 0; c < Columns; ++c) {
            grid->setColumnStretch(c, 1);
        }
        list->addWidget(Holo::card(grid));
    }
    list->addStretch();

    auto* layout = Holo::pageLayout(this, tr("Blueprints"));
    layout->addWidget(Holo::scrollArea(content), 1);
}

} // namespace glass
