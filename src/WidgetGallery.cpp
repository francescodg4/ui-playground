#include "WidgetGallery.hpp"

#include "Icons.hpp"
#include "ThemeRegistry.hpp"
#include "WidgetStyle.hpp"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDial>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLCDNumber>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTime>
#include <QToolButton>
#include <QTreeWidget>

namespace {

constexpr int IconRole = Qt::UserRole + 1;

} // namespace

WidgetGallery::WidgetGallery(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Widget gallery"));

    // ---- menus
    QMenu* file = menuBar()->addMenu(tr("&File"));
    QAction* quit = file->addAction(tr("&Quit"));
    quit->setShortcut(QKeySequence::Quit);
    connect(quit, &QAction::triggered, qApp, &QApplication::closeAllWindows);

    QMenu* interfaces = menuBar()->addMenu(tr("&Interface"));
    m_themeActions = new QActionGroup(this);
    int n = 0;
    for (const ThemeEntry& theme : Themes::all()) {
        QAction* action = interfaces->addAction(theme.name);
        action->setCheckable(true);
        action->setData(theme.id);
        action->setShortcut(QKeySequence(Qt::CTRL | (Qt::Key_1 + n++)));
        m_themeActions->addAction(action);
        connect(action, &QAction::triggered, this, [this, id = theme.id] { select(id); });
    }

    QMenu* view = menuBar()->addMenu(tr("&View"));
    m_darkAction = view->addAction(tr("Dar&k mode"));
    m_darkAction->setCheckable(true);
    m_darkAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    QAction* disable = view->addAction(tr("&Disable widgets"));
    disable->setCheckable(true);

    QMenu* help = menuBar()->addMenu(tr("&Help"));
    connect(help->addAction(tr("&About")), &QAction::triggered, this, [this] {
        QMessageBox::about(this, tr("Widget gallery"),
            tr("The standard Qt widgets drawn by each interface. "
               "Pick an interface to restyle everything at once."));
    });

    // ---- showcase
    auto* central = new QWidget;
    auto* grid = new QGridLayout(central);
    grid->setContentsMargins(16, 16, 16, 16);
    grid->setSpacing(16); // bento gap
    grid->addWidget(interfaceBox(), 0, 0, 1, 3);
    m_boxes = { buttonsBox(), inputBox(), rangesBox(), viewsBox(), telemetryBox() };
    grid->addWidget(m_boxes[0], 1, 0);
    grid->addWidget(m_boxes[1], 1, 1);
    grid->addWidget(m_boxes[2], 1, 2);
    grid->addWidget(m_boxes[3], 2, 0, 1, 2);
    grid->addWidget(m_boxes[4], 2, 2);
    for (int c = 0; c < 3; ++c) {
        grid->setColumnStretch(c, 1);
    }
    grid->setRowStretch(2, 1);
    setCentralWidget(central);
    statusBar()->setSizeGripEnabled(false);

    connect(m_disable, &QCheckBox::toggled, disable, &QAction::setChecked);
    connect(disable, &QAction::toggled, m_disable, &QCheckBox::setChecked);
    connect(m_disable, &QCheckBox::toggled, this, [this](bool on) { setWidgetsEnabled(!on); });

    // dark mode: the user's choice is applied and remembered
    m_dark = Themes::savedDark();
    const auto chooseDark = [this](bool on) {
        setDark(on);
        Themes::saveDark(on);
    };
    connect(m_darkMode, &QCheckBox::toggled, this, chooseDark);
    connect(m_darkAction, &QAction::triggered, this, chooseDark);

    connect(&m_ticker, &QTimer::timeout, this, [this] {
        m_clock->display(QTime::currentTime().toString(QStringLiteral("hh:mm:ss")));
        m_scan->setValue((m_scan->value() + 3) % 101);
    });
    m_ticker.start(1000);
    m_clock->display(QTime::currentTime().toString(QStringLiteral("hh:mm:ss")));

    resize(1200, 860);
}

void WidgetGallery::setTheme(const QString& id)
{
    const ThemeEntry* theme = Themes::find(id);
    if (!theme) {
        return;
    }
    m_theme = id;
    const bool dark = m_dark && theme->darkStyle;
    WidgetStyle* style = dark ? theme->darkStyle() : theme->widgetStyle();
    QApplication::setStyle(style); // the application owns it (and deletes the previous one)
    QApplication::setPalette(style->standardPalette());
    QApplication::setFont(style->font());

    {
        const QSignalBlocker block(m_selector);
        m_selector->setCurrentIndex(m_selector->findData(id));
    }
    for (QAction* action : m_themeActions->actions()) {
        action->setChecked(action->data().toString() == id);
    }
    // the dark mode switch applies to the designs that have one
    {
        const QSignalBlocker blockBox(m_darkMode);
        const QSignalBlocker blockAction(m_darkAction);
        m_darkMode->setChecked(dark); // the preference is kept for the designs that have a dark mode
        m_darkAction->setChecked(dark);
    }
    m_darkMode->setEnabled(theme->darkStyle != nullptr);
    m_darkAction->setEnabled(theme->darkStyle != nullptr);
    m_darkMode->setToolTip(theme->darkStyle ? tr("Switch this design between light and dark (Ctrl+D)") : tr("This design has no dark mode"));
    m_description->setText(theme->description);
    statusBar()->showMessage(dark ? tr("Interface: %1 (dark)").arg(theme->name) : tr("Interface: %1").arg(theme->name));
    tintIcons();
    update();
}

void WidgetGallery::setDark(bool dark)
{
    m_dark = dark;
    if (!m_theme.isEmpty()) {
        setTheme(m_theme);
    }
}

void WidgetGallery::select(const QString& id)
{
    setTheme(id);
    Themes::save(id);
}

void WidgetGallery::setWidgetsEnabled(bool enabled)
{
    for (QGroupBox* box : std::as_const(m_boxes)) {
        box->setEnabled(enabled);
    }
}

void WidgetGallery::tintIcons()
{
    const QColor ink = QApplication::palette().color(QPalette::Text);
    for (int i = 0; i < m_list->count(); ++i) {
        QListWidgetItem* item = m_list->item(i);
        item->setIcon(Icons::icon(Icon(item->data(IconRole).toInt()), ink));
    }
    for (QTreeWidgetItemIterator it(m_tree); *it; ++it) {
        (*it)->setIcon(0, Icons::icon(Icon((*it)->data(0, IconRole).toInt()), ink));
    }
}

void WidgetGallery::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    if (const auto* style = qobject_cast<const WidgetStyle*>(this->style())) {
        style->window(p, this, rect());
    } else {
        p.fillRect(rect(), palette().window());
    }
}

// ---- showcase -----------------------------------------------------------------------------------

QGroupBox* WidgetGallery::interfaceBox()
{
    auto* box = new QGroupBox(tr("Interface"));
    m_selector = new QComboBox;
    for (const ThemeEntry& theme : Themes::all()) {
        m_selector->addItem(theme.name, theme.id);
    }
    m_selector->setToolTip(tr("The design used to draw every widget (Ctrl+1 … 9)"));
    connect(m_selector, &QComboBox::currentIndexChanged, this, [this] { select(m_selector->currentData().toString()); });

    m_description = new QLabel;
    m_description->setWordWrap(true);
    m_disable = new QCheckBox(tr("Disable widgets"));
    m_darkMode = new QCheckBox(tr("Dark mode"));

    auto* layout = new QHBoxLayout(box);
    layout->addWidget(new QLabel(tr("Design")));
    layout->addWidget(m_selector);
    layout->addSpacing(8);
    layout->addWidget(m_description, 1);
    layout->addSpacing(8);
    layout->addWidget(m_darkMode);
    layout->addWidget(m_disable);
    return box;
}

QGroupBox* WidgetGallery::buttonsBox()
{
    auto* box = new QGroupBox(tr("Buttons"));
    auto* push = new QPushButton(tr("Push"));
    auto* toggle = new QPushButton(tr("Toggle"));
    toggle->setCheckable(true);
    toggle->setChecked(true);
    auto* flat = new QPushButton(tr("Flat"));
    flat->setFlat(true);
    auto* primary = new QPushButton(tr("Default"));
    primary->setDefault(true);
    auto* disabled = new QPushButton(tr("Disabled"));
    disabled->setEnabled(false);

    auto* scan = new QToolButton;
    scan->setText(tr("Scan"));
    scan->setPopupMode(QToolButton::MenuButtonPopup);
    scan->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* scanMenu = new QMenu(scan);
    scanMenu->addAction(tr("Scan area"));
    scanMenu->addAction(tr("Deep scan"));
    scanMenu->addSeparator();
    scanMenu->addAction(tr("Calibrate"))->setEnabled(false);
    scan->setMenu(scanMenu);

    auto* vehicles = new QVBoxLayout;
    auto* group = new QButtonGroup(box);
    for (const QString& name : { tr("Seamoth"), tr("Prawn suit"), tr("Cyclops") }) {
        auto* radio = new QRadioButton(name);
        group->addButton(radio);
        vehicles->addWidget(radio);
    }
    group->buttons().first()->setChecked(true);

    auto* options = new QVBoxLayout;
    auto* sonar = new QCheckBox(tr("Sonar"));
    sonar->setChecked(true);
    auto* lights = new QCheckBox(tr("Floodlights"));
    auto* dock = new QCheckBox(tr("Auto-dock"));
    dock->setTristate(true);
    dock->setCheckState(Qt::PartiallyChecked);
    options->addWidget(sonar);
    options->addWidget(lights);
    options->addWidget(dock);

    auto* layout = new QGridLayout(box);
    layout->addWidget(push, 0, 0);
    layout->addWidget(toggle, 0, 1);
    layout->addWidget(primary, 1, 0);
    layout->addWidget(flat, 1, 1);
    layout->addWidget(scan, 2, 0);
    layout->addWidget(disabled, 2, 1);
    layout->addLayout(vehicles, 3, 0);
    layout->addLayout(options, 3, 1);
    layout->setRowStretch(4, 1);
    return box;
}

QGroupBox* WidgetGallery::inputBox()
{
    auto* box = new QGroupBox(tr("Input"));
    auto* name = new QLineEdit;
    name->setPlaceholderText(tr("Diver's name"));
    name->setClearButtonEnabled(true);
    auto* code = new QLineEdit(QStringLiteral("lifepod5"));
    code->setEchoMode(QLineEdit::Password);
    auto* depth = new QSpinBox;
    depth->setRange(0, 1000);
    depth->setSingleStep(10);
    depth->setSuffix(tr(" m"));
    depth->setValue(120);
    auto* logged = new QDateTimeEdit(QDateTime(QDate(2026, 9, 24), QTime(9, 24)));
    logged->setDisplayFormat(QStringLiteral("yyyy-MM-dd  hh:mm"));
    auto* biome = new QComboBox;
    biome->addItems({ tr("Safe Shallows"), tr("Kelp Forest"), tr("Grassy Plateaus"), tr("Jellyshroom Cave"), tr("Lost River") });
    biome->setCurrentIndex(1);
    auto* search = new QComboBox;
    search->setEditable(true);
    search->addItems({ tr("Peeper"), tr("Bladderfish"), tr("Stalker"), tr("Reaper Leviathan") });
    search->setCurrentIndex(-1);
    search->lineEdit()->setPlaceholderText(tr("Search the databank"));

    auto* layout = new QFormLayout(box);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    layout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->addRow(tr("Name"), name);
    layout->addRow(tr("Passcode"), code);
    layout->addRow(tr("Depth"), depth);
    layout->addRow(tr("Logged"), logged);
    layout->addRow(tr("Biome"), biome);
    layout->addRow(tr("Search"), search);
    return box;
}

QGroupBox* WidgetGallery::rangesBox()
{
    auto* box = new QGroupBox(tr("Ranges"));
    auto* volume = new QSlider(Qt::Horizontal);
    volume->setRange(0, 100);
    volume->setValue(65);
    volume->setTickPosition(QSlider::TicksBelow);
    volume->setTickInterval(10);

    m_scan = new QProgressBar;
    m_scan->setRange(0, 100);
    m_scan->setValue(72);
    m_scan->setFormat(tr("Scanning %p%"));
    m_scan->setAlignment(Qt::AlignCenter);

    auto* scroll = new QScrollBar(Qt::Horizontal);
    scroll->setRange(0, 100);
    scroll->setPageStep(25);
    scroll->setValue(30);

    // a small equalizer of vertical sliders
    auto* eq = new QGridLayout;
    eq->setHorizontalSpacing(4);
    eq->setVerticalSpacing(4);
    const QStringList bands = { QStringLiteral("60"), QStringLiteral("310"), QStringLiteral("1K"), QStringLiteral("3K"), QStringLiteral("12K"), QStringLiteral("16K") };
    const int gains[] = { 5, 8, 2, -3, -6, 4 };
    for (int i = 0; i < bands.size(); ++i) {
        auto* band = new QSlider(Qt::Vertical);
        band->setRange(-12, 12);
        band->setValue(gains[i]);
        band->setTickPosition(QSlider::TicksBothSides);
        band->setTickInterval(6);
        band->setMinimumHeight(110);
        auto* label = new QLabel(bands[i]);
        label->setAlignment(Qt::AlignCenter);
        eq->addWidget(band, 0, i, Qt::AlignHCenter);
        eq->addWidget(label, 1, i);
    }

    auto* layout = new QVBoxLayout(box);
    layout->addWidget(new QLabel(tr("Volume")));
    layout->addWidget(volume);
    layout->addWidget(m_scan);
    layout->addWidget(scroll);
    layout->addLayout(eq, 1);
    return box;
}

QGroupBox* WidgetGallery::viewsBox()
{
    auto* box = new QGroupBox(tr("Views"));
    auto* tabs = new QTabWidget;
    tabs->setDocumentMode(false);

    // table
    struct Row {
        const char* item;
        int qty;
        int depth;
        bool tracked;
    };
    const Row rows[] = {
        { "Titanium", 12, 30, true },
        { "Copper ore", 4, 45, false },
        { "Quartz", 7, 60, true },
        { "Silver ore", 2, 120, false },
        { "Lithium", 5, 210, true },
        { "Diamond", 1, 350, false },
        { "Ruby", 3, 480, false },
    };
    auto* table = new QTableWidget(int(std::size(rows)), 4);
    table->setHorizontalHeaderLabels({ tr("Resource"), tr("Qty"), tr("Depth"), tr("Tracked") });
    for (int r = 0; r < int(std::size(rows)); ++r) {
        table->setItem(r, 0, new QTableWidgetItem(tr(rows[r].item)));
        auto* qty = new QTableWidgetItem(QString::number(rows[r].qty));
        qty->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        table->setItem(r, 1, qty);
        auto* depth = new QTableWidgetItem(tr("%1 m").arg(rows[r].depth));
        depth->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        table->setItem(r, 2, depth);
        auto* tracked = new QTableWidgetItem;
        tracked->setCheckState(rows[r].tracked ? Qt::Checked : Qt::Unchecked);
        table->setItem(r, 3, tracked);
    }
    table->verticalHeader()->hide();
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->setShowGrid(false);
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->selectRow(2);

    // tree
    m_tree = new QTreeWidget;
    m_tree->setHeaderLabels({ tr("Entry"), tr("Class") });
    const struct {
        const char* topic;
        Icon icon;
        QList<QPair<const char*, const char*>> entries;
    } topics[] = {
        { "Fauna", Icon::Book, { { "Peeper", "Herbivore" }, { "Bladderfish", "Herbivore" }, { "Reaper Leviathan", "Leviathan" } } },
        { "Flora", Icon::Book, { { "Creepvine", "Kelp" }, { "Acid mushroom", "Fungus" } } },
        { "Data", Icon::Book, { { "Lifepod 5 log", "Audio" } } },
    };
    for (const auto& topic : topics) {
        auto* parent = new QTreeWidgetItem(m_tree, { tr(topic.topic) });
        parent->setData(0, IconRole, int(topic.icon));
        for (const auto& [entry, kind] : topic.entries) {
            auto* child = new QTreeWidgetItem(parent, { tr(entry), tr(kind) });
            child->setData(0, IconRole, int(Icon::Doc));
        }
    }
    m_tree->expandAll();
    m_tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tree->setCurrentItem(m_tree->topLevelItem(0)->child(0));

    // icon list
    m_list = new QListWidget;
    m_list->setViewMode(QListView::IconMode);
    m_list->setIconSize(QSize(36, 36));
    m_list->setGridSize(QSize(96, 76));
    m_list->setMovement(QListView::Static);
    m_list->setResizeMode(QListView::Adjust);
    m_list->setWordWrap(true);
    const QPair<Icon, const char*> items[] = {
        { Icon::Titanium, "Titanium" }, { Icon::Ingot, "Ingot" }, { Icon::Glass, "Glass" }, { Icon::Battery, "Battery" },
        { Icon::Knife, "Knife" }, { Icon::Scanner, "Scanner" }, { Icon::Flashlight, "Flashlight" }, { Icon::Repair, "Repair tool" },
        { Icon::Tank, "O₂ tank" }, { Icon::Fins, "Fins" }, { Icon::Mask, "Mask" }, { Icon::Compass, "Compass" },
    };
    for (const auto& [icon, name] : items) {
        auto* item = new QListWidgetItem(tr(name), m_list);
        item->setData(IconRole, int(icon));
    }
    m_list->setCurrentRow(3);

    // rich text
    auto* text = new QTextEdit;
    text->setHtml(tr("<h3>Lifepod 5 &mdash; day 3</h3>"
                     "<p>Oxygen and power are <b>stable</b>. The fabricator is online; the radio still needs "
                     "<i>repairing</i> before we can answer the distress calls.</p>"
                     "<ul><li>Scan the wrecks in the Kelp Forest</li><li>Collect copper for batteries</li>"
                     "<li>Avoid the Reaper's territory north-west</li></ul>"));

    tabs->addTab(table, tr("Table"));
    tabs->addTab(m_tree, tr("Tree"));
    tabs->addTab(m_list, tr("List"));
    tabs->addTab(text, tr("Text"));

    auto* layout = new QVBoxLayout(box);
    layout->addWidget(tabs);
    return box;
}

QGroupBox* WidgetGallery::telemetryBox()
{
    auto* box = new QGroupBox(tr("Telemetry"));
    m_clock = new QLCDNumber(8);
    m_clock->setFrameShape(QFrame::StyledPanel);
    m_clock->setMinimumHeight(64);

    auto* gain = new QDial;
    gain->setRange(0, 100);
    gain->setValue(42);
    gain->setFixedSize(112, 112);
    auto* readout = new QLCDNumber(3);
    readout->setFrameShape(QFrame::StyledPanel);
    readout->setMinimumHeight(44);
    readout->display(gain->value());
    auto* signal = new QProgressBar;
    signal->setRange(0, 100);
    signal->setValue(gain->value());
    signal->setFormat(tr("%v dB"));
    signal->setAlignment(Qt::AlignCenter);
    connect(gain, &QDial::valueChanged, readout, qOverload<int>(&QLCDNumber::display));
    connect(gain, &QDial::valueChanged, signal, &QProgressBar::setValue);

    auto* side = new QVBoxLayout;
    side->addWidget(new QLabel(tr("Sonar gain")));
    side->addWidget(readout);
    side->addWidget(signal);
    side->addStretch(1);

    auto* dialRow = new QHBoxLayout;
    dialRow->addWidget(gain);
    dialRow->addLayout(side, 1);

    auto* layout = new QVBoxLayout(box);
    layout->addWidget(new QLabel(tr("Mission clock")));
    layout->addWidget(m_clock);
    layout->addLayout(dialRow);
    layout->addStretch(1);
    return box;
}
