#include "BlueprintsPage.hpp"

#include "Icons.hpp"
#include "Theme.hpp"
#include "widgets/Holo.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
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

/// Round token with the item icon and its name underneath.
class BlueprintTile : public QWidget {
public:
    explicit BlueprintTile(const Blueprint& bp)
        : m_bp(bp)
    {
        setToolTip(QString::fromLatin1(bp.name));
        setMinimumSize(96, 100);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setAttribute(Qt::WA_Hover);
        setCursor(Qt::PointingHandCursor);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const qreal d = 58;
        const QRectF disc(width() / 2.0 - d / 2, 2, d, d);

        if (underMouse()) {
            p.setPen(QPen(QColor(140, 220, 255, 110), 6));
            p.drawEllipse(disc);
        }
        QRadialGradient g(disc.center() - QPointF(0, d * 0.1), d * 0.6);
        g.setColorAt(0, QColor(170, 225, 255, 130));
        g.setColorAt(1, QColor(40, 115, 190, 90));
        p.setBrush(g);
        p.setPen(QPen(QColor(215, 240, 255, 165), 1.5));
        p.drawEllipse(disc);
        Icons::paint(p, m_bp.icon, disc.adjusted(11, 11, -11, -11), m_bp.accent);

        if (m_bp.isNew) {
            p.save();
            const QPointF c = disc.topRight() + QPointF(-6, 6);
            p.setPen(Qt::NoPen);
            p.setBrush(Theme::badge);
            p.drawEllipse(c, 7.5, 7.5);
            QFont f = font();
            f.setBold(true);
            f.setPixelSize(10);
            p.setFont(f);
            p.setPen(QColor(0x3b, 0x24, 0x00));
            p.drawText(QRectF(c.x() - 8, c.y() - 8, 16, 16), Qt::AlignCenter, QStringLiteral("!"));
            p.restore();
        }

        p.setPen(Theme::text);
        p.drawText(QRectF(0, disc.bottom() + 5, width(), height() - disc.bottom() - 5),
            Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, QString::fromLatin1(m_bp.name));
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
    list->setContentsMargins(0, 0, 12, 0);
    list->setSpacing(12);

    for (const Category& cat : categories()) {
        auto* header = new QLabel(QString::fromLatin1(cat.name));
        header->setObjectName(QStringLiteral("category"));
        list->addWidget(header);

        auto* grid = new QGridLayout;
        grid->setHorizontalSpacing(6);
        grid->setVerticalSpacing(10);
        for (int i = 0; i < cat.items.size(); ++i) {
            grid->addWidget(new BlueprintTile(cat.items[i]), i / Columns, i % Columns);
        }
        for (int c = 0; c < Columns; ++c) {
            grid->setColumnStretch(c, 1);
        }
        list->addLayout(grid);
    }
    list->addStretch();

    auto* layout = Holo::pageLayout(this, tr("Blueprints"));
    layout->addWidget(Holo::scrollArea(content), 1);
}
