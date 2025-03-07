#include "LogPage.hpp"

#include "Theme.hpp"
#include "widgets/Metal.hpp"
#include "widgets/MetalWidgets.hpp"
#include "widgets/Telemetry.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QVBoxLayout>

#include <algorithm>

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

/// Playlist row on the LCD; the playing one is highlighted.
class LogRow : public QWidget {
public:
    void setPlaying(bool playing)
    {
        m_playing = playing;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        if (m_playing) {
            QPainter p(this);
            p.fillRect(rect(), Theme::lcdRow);
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
    setStatusTip(tr("%1 unread messages").arg(unreadCount()));

    auto* content = new QWidget;
    auto* list = new QVBoxLayout(content);
    list->setContentsMargins(0, 0, 8, 0);
    list->setSpacing(2);

    int number = 0;
    for (const Day& day : logDays()) {
        auto* title = MetalUi::lcdLabel(QString::fromLatin1(day.title).toUpper());
        title->setFont(Metal::digitalFont(13, true));
        list->addWidget(title);
        for (const Entry& entry : day.entries) {
            const QString text = QString::fromLatin1(entry.text);
            auto* row = new LogRow;
            auto* index = MetalUi::lcdLabel(QStringLiteral("%1.").arg(++number));
            index->setFont(Metal::digitalFont(12));
            index->setFixedWidth(26);
            index->setAlignment(Qt::AlignTop | Qt::AlignRight);
            auto* label = MetalUi::lcdLabel(text);
            label->setFont(Metal::digitalFont(12));
            label->setWordWrap(true);
            label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            auto* layout = new QHBoxLayout(row);
            layout->setContentsMargins(4, 5, 6, 5);
            layout->setSpacing(10);
            layout->addWidget(index, 0, Qt::AlignTop);
            layout->addWidget(label, 1);
            if (entry.audio) {
                auto* play = new RoundButton(Metal::Glyph::Play, 30);
                play->setToolTip(tr("Play"));
                connect(play, &RoundButton::clicked, this, [this, play, row, text] { togglePlayback(play, row, text); });
                layout->addWidget(play, 0, Qt::AlignTop);
            } else {
                layout->addSpacing(30);
            }
            list->addWidget(row);
        }
        list->addSpacing(6);
    }
    list->addStretch();

    m_playback.setSingleShot(true);
    connect(&m_playback, &QTimer::timeout, this, [this] { setPlaying(m_playing, m_playingRow, false); });

    auto* panel = new LcdPanel;
    auto* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(12, 8, 6, 8);
    panelLayout->addWidget(MetalUi::scrollArea(content));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(panel);
}

void LogPage::togglePlayback(RoundButton* button, LogRow* row, const QString& text)
{
    const bool wasPlaying = m_playing == button;
    setPlaying(m_playing, m_playingRow, false);
    if (!wasPlaying) {
        // no audio backend in the demo: "play" for roughly the time it takes to read the entry
        setPlaying(button, row, true);
        Telemetry::report(text);
        m_playback.start(int(1500 + 55 * text.size()));
    }
}

void LogPage::setPlaying(RoundButton* button, LogRow* row, bool playing)
{
    if (!button) {
        return;
    }
    button->setGlyph(playing ? Metal::Glyph::Stop : Metal::Glyph::Play);
    button->setActive(playing);
    button->setToolTip(playing ? tr("Stop") : tr("Play"));
    row->setPlaying(playing);
    Telemetry::setPlaying(playing);
    if (playing) {
        m_playing = button;
        m_playingRow = row;
    } else if (m_playing == button) {
        m_playing = nullptr;
        m_playingRow = nullptr;
        m_playback.stop();
        Telemetry::report(statusTip());
    }
}
