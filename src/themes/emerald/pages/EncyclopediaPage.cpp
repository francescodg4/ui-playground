#include "EncyclopediaPage.hpp"

#include "Icons.hpp"
#include "themes/emerald/Theme.hpp"
#include "themes/emerald/widgets/Pixel.hpp"
#include "themes/emerald/widgets/PixelWidgets.hpp"

#include <QGuiApplication>
#include <QHBoxLayout>
#include <QPainterPath>
#include <QScrollArea>
#include <QStyledItemDelegate>
#include <QTreeWidget>

namespace emerald {

using Pixel::Scale;

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

/// Tree rows on the party deck: bi-colour arrows for folders, highlighted tile for the open entry.
class TopicDelegate : public QStyledItemDelegate {
public:
    explicit TopicDelegate(QTreeWidget* tree)
        : QStyledItemDelegate(tree)
        , m_tree(tree)
    {
    }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override { return QSize(100 * Scale, 14 * Scale); }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        int depth = 0;
        for (QModelIndex i = index.parent(); i.isValid(); i = i.parent()) {
            ++depth;
        }
        const bool folder = index.model()->hasChildren(index);
        const bool open = index.data(OpenEntryRole).toBool();
        const bool hover = option.state & QStyle::State_MouseOver;

        Pixel::Canvas canvas(*painter, option.rect);
        QPainter& p = canvas.p();
        const QRect row = canvas.rect().adjusted(0, 1, -1, -1);
        if (open) {
            Pixel::highlightTile(p, row);
        } else if (hover) {
            Pixel::chamfer(p, row, Theme::primaryGreen.lighter(118), Theme::primaryGreen.lighter(118), 1);
        }
        const int x = 3 + depth * 8;
        if (folder) {
            const bool expanded = m_tree->isExpanded(index);
            Pixel::arrow(p, expanded ? QRect(x, row.center().y() - 2, 7, 6) : QRect(x + 1, row.center().y() - 3, 6, 7),
                expanded ? Pixel::Direction::Down : Pixel::Direction::Right);
        }
        Pixel::text(p, QRect(x + 10, row.top(), row.width() - x - 10, row.height()), Qt::AlignLeft | Qt::AlignVCenter,
            index.data().toString(), open ? Theme::text : Theme::lightText, open ? Theme::textShadow : Theme::darkGreen);
    }

private:
    QTreeWidget* m_tree;
};

/// The scan card drawn at full detail, then reduced to the CRT's resolution.
QImage scanImage(int hue)
{
    QImage image(400, 200, QImage::Format_RGB32);
    QPainter p(&image);
    p.setRenderHint(QPainter::Antialiasing);
    const auto tint = [hue](int lightness) { return QColor::fromHsl(hue % 360, 150, lightness * 255 / 100); };
    const QColor hud(0x9f, 0xe6, 0xff);

    p.fillRect(QRectF(0, 0, 400, 200), Theme::crtDark);

    p.setPen(QPen(QColor(45, 111, 154, 150), 0.5));
    for (int y = 0; y <= 200; y += 25) {
        p.drawLine(0, y, 400, y);
    }
    for (int x = 0; x <= 400; x += 25) {
        p.drawLine(x, 0, x, 200);
    }

    QFont f;
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

    f.setFamily(QFont().family());
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

    return image;
}

} // namespace

/// Data window whose CRT screen shows the bio scan of the open entry.
class ScanScreen : public QWidget {
public:
    ScanScreen()
    {
        QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        policy.setHeightForWidth(true);
        setSizePolicy(policy);
    }

    void setHue(int hue)
    {
        m_scan = scanImage(hue);
        m_cache = {};
        update();
    }

    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int w) const override { return (w / Scale / 2 + 24) * Scale; }
    QSize sizeHint() const override { return QSize(220 * Scale, heightForWidth(220 * Scale)); }

protected:
    void paintEvent(QPaintEvent*) override
    {
        Pixel::Canvas canvas(this);
        QPainter& p = canvas.p();
        const QRect screen = Pixel::dataWindow(p, canvas.rect(), tr("SCAN DATA"));
        if (m_cache.size() != screen.size()) {
            m_cache = Pixel::pixelate(m_scan, screen.size());
        }
        p.drawImage(screen.topLeft(), m_cache);
        Pixel::scanlines(p, screen);
    }

private:
    QImage m_scan;
    QImage m_cache;
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
    m_tree->setCursor(Qt::PointingHandCursor);

    auto* deck = new DeckPanel;
    deck->setMinimumWidth(130 * Scale);
    auto* deckLayout = new QVBoxLayout(deck);
    deckLayout->setContentsMargins(5 * Scale, 5 * Scale, 5 * Scale, 5 * Scale);
    deckLayout->addWidget(m_tree);

    m_scan = new ScanScreen;
    auto* textBox = new PaperBox;
    m_title = new PixelLabel(QString(), PixelLabel::Tone::Dark, true);
    m_body = new PixelLabel;
    m_body->setWordWrap(true);
    auto* textLayout = new QVBoxLayout(textBox);
    textLayout->setContentsMargins(8 * Scale, 5 * Scale, 8 * Scale, 6 * Scale);
    textLayout->setSpacing(3 * Scale);
    textLayout->addWidget(m_title);
    textLayout->addWidget(m_body);

    auto* entry = new QWidget;
    auto* entryLayout = new QVBoxLayout(entry);
    entryLayout->setContentsMargins(0, 0, 6 * Scale, 0);
    entryLayout->setSpacing(4 * Scale);
    entryLayout->addWidget(m_scan);
    entryLayout->addWidget(textBox);
    entryLayout->addStretch();

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4 * Scale, 2 * Scale, 2 * Scale, 2 * Scale);
    layout->setSpacing(6 * Scale);
    layout->addWidget(deck, 2);
    layout->addWidget(PixelUi::scrollArea(entry), 3);

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
    m_title->setText(item->text(0).toUpper());
    const QString text = item->data(0, TextRole).toString();
    m_body->setText(text.isEmpty() ? tr("Scan data incomplete. Scan a live specimen with the scanner to complete this entry.") : text);
    m_tree->viewport()->update();
}

} // namespace emerald
