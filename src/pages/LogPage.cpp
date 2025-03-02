#include "LogPage.hpp"

#include "Icons.hpp"
#include "Theme.hpp"
#include "widgets/Holo.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStyle>
#include <QToolButton>

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
    auto* grid = new QGridLayout(content);
    grid->setContentsMargins(4, 4, 30, 4);
    grid->setHorizontalSpacing(22);
    grid->setVerticalSpacing(24);
    grid->setColumnMinimumWidth(0, 12);
    grid->setColumnStretch(2, 1);

    int row = 0;
    for (const Day& day : logDays()) {
        auto* title = new QLabel(QString::fromLatin1(day.title));
        title->setObjectName(QStringLiteral("day"));
        grid->addWidget(title, row++, 0, 1, 4);

        for (const Entry& entry : day.entries) {
            const QString text = QString::fromLatin1(entry.text);
            auto* doc = new QLabel;
            doc->setPixmap(Icons::pixmap(Icon::LogDoc, QSize(22, 30), Theme::green));
            auto* label = new QLabel(text);
            label->setWordWrap(true);
            grid->addWidget(doc, row, 1, Qt::AlignVCenter);
            grid->addWidget(label, row, 2);
            if (entry.audio) {
                auto* play = new QToolButton;
                play->setObjectName(QStringLiteral("logPlay"));
                play->setIcon(Icons::icon(Icon::Play, Theme::green));
                play->setIconSize(QSize(20, 20));
                play->setFixedSize(36, 36);
                play->setToolTip(tr("Play"));
                play->setCursor(Qt::PointingHandCursor);
                play->setFocusPolicy(Qt::NoFocus);
                connect(play, &QToolButton::clicked, this, [this, play, text] { togglePlayback(play, text); });
                grid->addWidget(play, row, 3, Qt::AlignVCenter);
            } else {
                grid->addItem(new QSpacerItem(36, 36), row, 3);
            }
            ++row;
        }
    }
    grid->setRowStretch(row, 1);

    m_playback.setSingleShot(true);
    connect(&m_playback, &QTimer::timeout, this, [this] { setPlaying(m_playing, false); });

    auto* layout = Holo::pageLayout(this, tr("Log"));
    layout->addWidget(Holo::scrollArea(content), 1);
}

void LogPage::togglePlayback(QToolButton* button, const QString& text)
{
    const bool wasPlaying = m_playing == button;
    setPlaying(m_playing, false);
    if (!wasPlaying) {
        // no audio backend in the demo: "play" for roughly the time it takes to read the entry
        setPlaying(button, true);
        m_playback.start(int(1500 + 55 * text.size()));
    }
}

void LogPage::setPlaying(QToolButton* button, bool playing)
{
    if (!button) {
        return;
    }
    button->setIcon(Icons::icon(playing ? Icon::Stop : Icon::Play, Theme::green));
    button->setToolTip(playing ? tr("Stop") : tr("Play"));
    button->setProperty("playing", playing);
    button->style()->unpolish(button);
    button->style()->polish(button);
    if (playing) {
        m_playing = button;
    } else if (m_playing == button) {
        m_playing = nullptr;
        m_playback.stop();
    }
}
