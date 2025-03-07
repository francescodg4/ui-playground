#include "LogPage.hpp"

#include "Icons.hpp"
#include "themes/emerald/Theme.hpp"
#include "themes/emerald/widgets/Pixel.hpp"
#include "themes/emerald/widgets/PixelWidgets.hpp"

#include <QHBoxLayout>
#include <QScrollArea>
#include <QVBoxLayout>

#include <algorithm>

namespace emerald {

using Pixel::Scale;

namespace {

struct Entry {
    const char* text;
    bool audio = true;
    bool unread = false;
};

struct Day {
    const char* title;
    QList<Entry> entries;
};

const QList<Day>& logDays()
{
    static const QList<Day> days = {
        { "Day 1",
            {
                { "The fabricator draws on stored data to produce equipment suited to the environment, "
                  "using local materials. This safety setting cannot be changed.",
                    true, true },
                { "Environment: uncharted ocean planet. Oxygen/nitrogen atmosphere. Water contamination: high.", true, true },
                { "RADIO: automated distress signal received from Lifepod 12, coordinates attached. "
                  "The lifepod has sunk below safe diving depth. Do not attempt recovery without a submersible.",
                    false, true },
                { "The Aurora suffered a hull failure in orbit. Cause unknown. No human life signs detected.", true, true },
            } },
        { "Day 2",
            {
                { "Photo acquisition module online. Captured images are stored in the Photo Manager." },
                { "Scanner calibrated. Scan unknown lifeforms to add them to the encyclopedia." },
            } },
    };
    return days;
}

} // namespace

/// One message on lined paper; while it plays the glove points at it.
class LogRow : public PaperBox {
public:
    void setPlaying(bool playing)
    {
        m_playing = playing;
        update();
    }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        PaperBox::paintEvent(event);
        if (m_playing) {
            Pixel::Canvas canvas(this);
            Pixel::hand(canvas.p(), QPoint(15, canvas.rect().center().y()), Pixel::Direction::Right);
        }
    }

private:
    bool m_playing = false;
};

int LogPage::unreadCount()
{
    int count = 0;
    for (const Day& day : logDays()) {
        count += int(std::count_if(day.entries.begin(), day.entries.end(), [](const Entry& e) { return e.unread; }));
    }
    return count;
}

LogPage::LogPage(QWidget* parent)
    : QWidget(parent)
{
    auto* content = new QWidget;
    auto* list = new QVBoxLayout(content);
    list->setContentsMargins(2 * Scale, 2 * Scale, 6 * Scale, 2 * Scale);
    list->setSpacing(4 * Scale);

    for (const Day& day : logDays()) {
        list->addWidget(new BannerStrip(QString::fromLatin1(day.title).toUpper(), Pixel::Wallpaper::Beach));
        for (const Entry& entry : day.entries) {
            const QString text = QString::fromLatin1(entry.text);
            auto* row = new LogRow;
            auto* label = new PixelLabel(text);
            label->setWordWrap(true);
            auto* layout = new QHBoxLayout(row);
            layout->setContentsMargins(18 * Scale, 5 * Scale, 8 * Scale, 5 * Scale);
            layout->setSpacing(8 * Scale);
            layout->addWidget(new SpriteLabel(Icon::LogDoc, 14, Theme::primaryGreen), 0, Qt::AlignVCenter);
            layout->addWidget(label, 1);
            if (entry.audio) {
                auto* play = new PixelKey(tr("PLAY"), Theme::keyYellow);
                connect(play, &PixelKey::clicked, this, [this, play, row, text] { togglePlayback(play, row, text); });
                layout->addWidget(play, 0, Qt::AlignVCenter);
            } else {
                layout->addSpacing(Pixel::keyWidth(tr("PLAY")) * Scale);
            }
            list->addWidget(row);
        }
    }
    list->addStretch();

    m_playback.setSingleShot(true);
    connect(&m_playback, &QTimer::timeout, this, [this] { setPlaying(m_playing, m_playingRow, false); });

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4 * Scale, 2 * Scale, 2 * Scale, 2 * Scale);
    layout->addWidget(PixelUi::scrollArea(content));
}

void LogPage::togglePlayback(PixelKey* key, LogRow* row, const QString& text)
{
    const bool wasPlaying = m_playing == key;
    setPlaying(m_playing, m_playingRow, false);
    if (!wasPlaying) {
        // no audio backend in the demo: "play" for roughly the time it takes to read the entry
        setPlaying(key, row, true);
        m_playback.start(int(1500 + 55 * text.size()));
    }
}

void LogPage::setPlaying(PixelKey* key, LogRow* row, bool playing)
{
    if (!key) {
        return;
    }
    key->setLabel(playing ? tr("STOP") : tr("PLAY"));
    key->setFace(playing ? Theme::kbdLowerOrange : Theme::keyYellow);
    row->setPlaying(playing);
    if (playing) {
        m_playing = key;
        m_playingRow = row;
    } else if (m_playing == key) {
        m_playing = nullptr;
        m_playingRow = nullptr;
        m_playback.stop();
    }
}

} // namespace emerald
