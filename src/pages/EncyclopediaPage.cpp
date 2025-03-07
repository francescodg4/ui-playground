#include "EncyclopediaPage.hpp"

#include "Icons.hpp"
#include "Theme.hpp"
#include "widgets/Glass.hpp"
#include "widgets/Holo.hpp"

#include <QGuiApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>
#include <QStyledItemDelegate>
#include <QTreeWidget>

namespace {

constexpr int HueRole = Qt::UserRole;
constexpr int TextRole = Qt::UserRole + 1;
constexpr int OpenEntryRole = Qt::UserRole + 2; ///< true on the entry shown on the right

struct Node {
    const char* name;
    QList<Node> children = {};
    int hue = 200;
    const char* text = nullptr;
    bool open = false;
};

const Node Peeper = { "Peeper", {}, 260,
    "A small, fast fish found in large schools in the shallows.\n\n"
    "1. Oversized Eyes:\nThe forward-facing eyes see well in bright water and pick out colour easily, "
    "but the fish is almost blind once the light fades.\n\n"
    "2. Strong Fins:\nShort, powerful fins let it hold position against the currents that sweep the shallows.\n\n"
    "Assessment: Edible. Common food source." };

const QList<Node>& topics()
{
    static const QList<Node> nodes = {
        { "Advanced Theories", { { "Alien Architecture", {}, 150 }, { "Energy Signatures", {}, 190 } } },
        { "Data Downloads", { { "Aurora Crew Manifest", {}, 210 }, { "Lifepod Protocols", {}, 30 } } },
        { "Alien Lifeforms",
            {
                { "Coral", { { "Brain Coral", {}, 330 }, { "Table Coral", {}, 280 } } },
                { "Fauna",
                    {
                        { "Carnivores", { { "Stalker", {}, 200 }, { "Sand Shark", {}, 20 } } },
                        { "Deceased", { { "Giant Skeleton", {}, 40 } } },
                        { "Herbivores",
                            {
                                { "Bladderfish", {}, 90 },
                                { "Boomerang", {}, 20 },
                                { "Cute Fish", {}, 300 },
                                { "Eyeye", {}, 350 },
                                { "Crimson Ray", {}, 0 },
                                { "Garryfish", {}, 45 },
                                { "Gasopod", {}, 60 },
                                { "Ghostray", {}, 185 },
                                Peeper,
                            },
                            200, nullptr, true },
                    },
                    200, nullptr, true },
            },
            200, nullptr, true },
    };
    return nodes;
}

QTreeWidgetItem* addNodes(const QList<Node>& nodes, QTreeWidget* tree, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* peeper = nullptr;
    for (const Node& node : nodes) {
        auto* item = parent ? new QTreeWidgetItem(parent) : new QTreeWidgetItem(tree);
        item->setText(0, QString::fromLatin1(node.name));
        item->setData(0, HueRole, node.hue);
        if (node.text) {
            item->setData(0, TextRole, QString::fromLatin1(node.text));
        }
        if (QTreeWidgetItem* found = addNodes(node.children, tree, item)) {
            peeper = found;
        }
        item->setExpanded(node.open);
        if (qstrcmp(node.name, Peeper.name) == 0) {
            peeper = item;
        }
    }
    return peeper;
}

/// Draws each row as a translucent bar with a chevron for folders; indentation by depth.
class TopicDelegate : public QStyledItemDelegate {
public:
    explicit TopicDelegate(QTreeWidget* tree)
        : QStyledItemDelegate(tree)
        , m_tree(tree)
    {
    }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override { return QSize(200, 30); }

    void paint(QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        int depth = 0;
        for (QModelIndex i = index.parent(); i.isValid(); i = i.parent()) {
            ++depth;
        }
        const bool folder = index.model()->hasChildren(index);
        const bool open = index.data(OpenEntryRole).toBool();
        const bool hover = option.state & QStyle::State_MouseOver;
        const bool pressed = hover && (QGuiApplication::mouseButtons() & Qt::LeftButton);

        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        QRectF bar = QRectF(option.rect).adjusted(4, 2, -4, -2);
        if (pressed) {
            bar = Glass::scaled(bar, Theme::pressedScale);
        }
        const qreal radius = bar.height() / 2;
        // no structural outlines: the open entry radiates, hover lifts the row slightly
        if (open) {
            Glass::paintGlow(*p, bar, radius, Theme::glowAccent, 0.7);
            p->setBrush(QColor(0x3b, 0x9b, 0xff, 130));
        } else {
            p->setBrush(QColor(255, 255, 255, hover ? 26 : 10));
        }
        p->setPen(Qt::NoPen);
        p->drawRoundedRect(bar, radius, radius);

        const qreal x = bar.left() + 8 + depth * 14;
        if (folder) {
            const QPointF c(x + 5, bar.center().y());
            p->save();
            p->translate(c);
            p->rotate(m_tree->isExpanded(index) ? 90 : 0);
            Icons::paint(*p, Icon::ChevronRight, QRectF(-5, -5, 10, 10), Theme::text);
            p->restore();
        }
        p->setPen(Theme::text);
        p->setFont(option.font);
        p->drawText(QRectF(x + (folder ? 15 : 0) + 2, bar.top(), bar.width(), bar.height()), Qt::AlignVCenter,
            index.data().toString());
        p->restore();
    }

private:
    QTreeWidget* m_tree;
};

} // namespace

/// "Bio scan" card: HUD frame, readouts and a fish tinted with the entry's hue.
class BioScan : public QWidget {
public:
    BioScan()
    {
        QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        policy.setHeightForWidth(true);
        setSizePolicy(policy);
    }

    void setHue(int hue)
    {
        m_hue = hue;
        update();
    }

    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int w) const override { return w / 2; }
    QSize sizeHint() const override { return QSize(400, 200); }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const qreal s = std::min(width() / 400.0, height() / 200.0);
        p.translate((width() - 400 * s) / 2, (height() - 200 * s) / 2);
        p.scale(s, s);

        const auto tint = [this](int lightness) { return QColor::fromHsl(m_hue % 360, 150, lightness * 255 / 100); };
        const QColor hud(0x9f, 0xe6, 0xff);

        QPainterPath card;
        card.addRoundedRect(QRectF(0.5, 0.5, 399, 199), 16, 16);
        p.fillPath(card, QColor(0x0b, 0x26, 0x40));
        p.setClipPath(card);

        p.setPen(QPen(QColor(255, 255, 255, 16), 0.5));
        for (int y = 0; y <= 200; y += 25) {
            p.drawLine(0, y, 400, y);
        }
        for (int x = 0; x <= 400; x += 25) {
            p.drawLine(x, 0, x, 200);
        }

        QFont f = font();
        f.setBold(true);
        f.setPixelSize(9);
        p.setFont(f);
        p.setPen(hud);
        p.drawText(QPointF(10, 16), QStringLiteral("BIO SCAN"));
        p.drawText(QPointF(318, 16), QStringLiteral("THREAT LEVEL"));
        p.setPen(Qt::NoPen);
        for (int i = 0; i < 5; ++i) {
            p.setBrush(QColor(255, 106, 61, i < 1 ? 255 : 70));
            p.drawRect(QRectF(340 + i * 10, 30 - i * 2, 7, 4 + i * 2));
        }

        f.setBold(false);
        f.setPixelSize(7);
        f.setFamily(QStringLiteral("monospace"));
        p.setFont(f);
        p.setPen(QColor(0x6f, 0xbf, 0xe6));
        const char* readouts[] = { "MASS   0.8 KG", "LENGTH 0.3 M", "DEPTH  0-40 M", "DIET   PLANKTON" };
        for (int i = 0; i < 4; ++i) {
            p.drawText(QPointF(10, 40 + i * 10), QString::fromLatin1(readouts[i]));
        }

        // the fish
        p.save();
        p.translate(190, 105);
        p.fillPath(Icons::svgPath(u"M-70 0Q-20-55 40-20Q60 0 40 20Q-20 55-70 0Z"), tint(35));
        p.fillPath(Icons::svgPath(u"M-70 0L-105-28L-95 0L-105 28Z"), tint(28));
        p.fillPath(Icons::svgPath(u"M-20-35Q0-70 20-28ZM-20 35Q0 70 20 28Z"), tint(45));
        p.setBrush(QColor(0xf5, 0xb2, 0x1c));
        p.drawEllipse(QPointF(18, -2), 24, 24);
        p.setBrush(QColor(0x8a, 0x4a, 0x00));
        p.drawEllipse(QPointF(18, -2), 14, 14);
        p.setBrush(QColor(0x11, 0x11, 0x11));
        p.drawEllipse(QPointF(18, -2), 7, 7);
        p.setBrush(Qt::white);
        p.drawEllipse(QPointF(13, -7), 3, 3);
        p.restore();

        // radar, mini screen and numbered markers
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(0x7f, 0xdc, 0xff), 1));
        p.drawEllipse(QPointF(335, 120), 30, 30);
        p.setPen(QPen(QColor(0x7f, 0xdc, 0xff), 1, Qt::DashLine));
        p.drawEllipse(QPointF(335, 120), 20, 20);
        p.fillRect(QRectF(328, 113, 14, 14), tint(50));

        p.setPen(QPen(QColor(0x7f, 0xdc, 0xff), 1));
        p.setBrush(QColor(0x0e, 0x3b, 0x5a));
        p.drawRoundedRect(QRectF(18, 130, 90, 55), 3, 3);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(hud, 1));
        p.drawPath(Icons::svgPath(u"M22 175Q50 140 80 160T104 150"));

        f.setFamily(font().family());
        f.setBold(true);
        p.setFont(f);
        const QPointF markers[] = { { 250, 60 }, { 222, 160 }, { 300, 150 } };
        for (int i = 0; i < 3; ++i) {
            p.setPen(QPen(QColor(0x7f, 0xdc, 0xff), 1));
            p.setBrush(QColor(0x0b, 0x2a, 0x44));
            p.drawEllipse(markers[i], 6, 6);
            p.setPen(Qt::white);
            p.drawText(QRectF(markers[i].x() - 6, markers[i].y() - 6, 12, 12), Qt::AlignCenter, QString::number(i + 1));
        }

        p.setClipping(false);
        p.setBrush(Qt::NoBrush);
        QLinearGradient light(0, 0, 400, 200);
        light.setColorAt(0, Theme::glassHighlight);
        light.setColorAt(0.45, Theme::glassBorder);
        p.setPen(QPen(QBrush(light), 1));
        p.drawPath(card);
    }

private:
    int m_hue = 200;
};

EncyclopediaPage::EncyclopediaPage(QWidget* parent)
    : QWidget(parent)
{
    m_tree = new QTreeWidget;
    m_tree->setHeaderHidden(true);
    m_tree->setRootIsDecorated(false);
    m_tree->setIndentation(0);
    m_tree->setUniformRowHeights(true);
    m_tree->setExpandsOnDoubleClick(false);
    m_tree->setSelectionMode(QAbstractItemView::NoSelection);
    m_tree->setMouseTracking(true);
    m_tree->setFocusPolicy(Qt::NoFocus);
    m_tree->setItemDelegate(new TopicDelegate(m_tree));
    m_tree->viewport()->setAutoFillBackground(false);
    m_tree->setMinimumWidth(220);
    m_tree->setCursor(Qt::PointingHandCursor);

    m_scan = new BioScan;
    m_title = new QLabel;
    m_title->setObjectName(QStringLiteral("entryTitle"));
    m_body = new QLabel;
    m_body->setWordWrap(true);
    m_body->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    auto* entry = new QWidget;
    auto* entryLayout = new QVBoxLayout(entry);
    entryLayout->setContentsMargins(0, 0, 10, 0);
    entryLayout->setSpacing(10);
    entryLayout->addWidget(m_scan);
    entryLayout->addWidget(m_title);
    entryLayout->addWidget(m_body);
    entryLayout->addStretch();

    auto* treeCell = new QVBoxLayout;
    treeCell->addWidget(m_tree);
    auto* entryCell = new QVBoxLayout;
    entryCell->addWidget(Holo::scrollArea(entry));

    auto* columns = new QHBoxLayout;
    columns->setSpacing(Theme::gap);
    columns->addWidget(Holo::card(treeCell), 1);
    columns->addWidget(Holo::card(entryCell), 2);

    auto* layout = Holo::pageLayout(this, tr("Encyclopedia"));
    layout->addLayout(columns, 1);

    connect(m_tree, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item) {
        if (item->childCount() > 0) {
            item->setExpanded(!item->isExpanded());
        } else {
            showEntry(item);
        }
    });

    showEntry(addNodes(topics(), m_tree, nullptr));
}

void EncyclopediaPage::showEntry(QTreeWidgetItem* item)
{
    if (!item) {
        return;
    }
    // The open entry is marked on the item instead of using the tree selection: re-selecting it
    // would make QTreeView scroll to it and re-expand any folder above it that was just collapsed.
    if (m_entry) {
        m_entry->setData(0, OpenEntryRole, false);
    }
    m_entry = item;
    m_entry->setData(0, OpenEntryRole, true);
    m_scan->setHue(item->data(0, HueRole).toInt());
    m_title->setText(item->text(0));
    const QString text = item->data(0, TextRole).toString();
    m_body->setText(text.isEmpty() ? tr("Scan data incomplete. Scan a live specimen with the scanner to complete this entry.") : text);
}
