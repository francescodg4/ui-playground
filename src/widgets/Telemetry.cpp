#include "Telemetry.hpp"

#include "Theme.hpp"
#include "widgets/Metal.hpp"

#include <QPainter>
#include <QRandomGenerator>
#include <QTime>

#include <cmath>

namespace {
Telemetry* s_instance = nullptr;
constexpr int Bands = 19;
}

Telemetry::Telemetry(QWidget* parent)
    : QWidget(parent)
    , m_bands(Bands)
    , m_peaks(Bands)
{
    s_instance = this;
    setFixedHeight(92);
    for (int i = 0; i < Bands; ++i) { // a resting spectrum shape until the first frame
        m_bands[i] = 0.18 + 0.12 * std::sin(i * 0.7);
        m_peaks[i] = m_bands[i] + 0.08;
    }
    connect(&m_timer, &QTimer::timeout, this, &Telemetry::tick);
    m_timer.start(50);
}

Telemetry::~Telemetry()
{
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

void Telemetry::report(const QString& message)
{
    if (s_instance) {
        s_instance->m_message = message;
        s_instance->m_scroll = 0;
        s_instance->update();
    }
}

void Telemetry::setPlaying(bool playing)
{
    if (s_instance) {
        s_instance->m_playing = playing;
        if (playing) {
            s_instance->m_playClock.start();
        }
        s_instance->update();
    }
}

void Telemetry::setPage(const QString& name, const QString& status)
{
    m_page = name;
    m_message = status;
    m_scroll = 0;
    update();
}

void Telemetry::setCounter(const QString& label, int value)
{
    for (auto& counter : m_counters) {
        if (counter.first == label) {
            counter.second = value;
            update();
            return;
        }
    }
    m_counters.append({ label, value });
    update();
}

void Telemetry::setAnimated(bool animated)
{
    animated ? m_timer.start(50) : m_timer.stop();
}

void Telemetry::tick()
{
    m_scroll += 0.5;
    auto* rng = QRandomGenerator::global();
    for (int i = 0; i < Bands; ++i) {
        const qreal shape = 1.0 - std::abs(i - Bands * 0.35) / Bands; // more energy in the low-mid bands
        const qreal target = m_playing ? (0.35 + 0.65 * rng->generateDouble()) * shape
                                       : 0.08 + 0.18 * shape * (0.5 + 0.5 * std::sin(m_scroll * 0.15 + i * 0.6));
        m_bands[i] += (target - m_bands[i]) * 0.45;
        m_peaks[i] = std::max(m_peaks[i] - 0.02, m_bands[i]);
    }
    update();
}

void Telemetry::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    const QRectF panel = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    Metal::lcd(p, panel, Theme::radiusPanel);

    // play state and the digital clock (track elapsed while a log entry plays)
    Metal::glyph(p, QRectF(12, 20, 9, 11), m_playing ? Metal::Glyph::Play : Metal::Glyph::Stop, Theme::lcdGlow);
    QString clock = QTime::currentTime().toString(QStringLiteral("h:mm"));
    if (m_playing) {
        const qint64 s = m_playClock.elapsed() / 1000;
        clock = QStringLiteral("%1:%2").arg(s / 60).arg(s % 60, 2, 10, QLatin1Char('0'));
    }
    Metal::sevenSegment(p, QRectF(30, 10, 120, 32), clock, Theme::lcdGlow, Theme::lcdDim);

    // counters (like KBPS / KHZ) and the status badge (like STEREO)
    p.setFont(Metal::digitalFont(11, true));
    qreal x = 168;
    for (const auto& [label, value] : m_counters) {
        const qreal lw = p.fontMetrics().horizontalAdvance(label);
        Metal::glowText(p, QRectF(x, 10, lw, 14), Qt::AlignVCenter, label, Theme::lcdGlow);
        const QString v = QString::number(value);
        const QRectF box(x + lw + 5, 10, p.fontMetrics().horizontalAdvance(v) + 10, 14);
        p.setPen(Qt::NoPen);
        p.setBrush(Theme::lcdGlow);
        p.drawRoundedRect(box, 3, 3);
        p.setPen(Theme::lcdBg);
        p.drawText(box, Qt::AlignCenter, v);
        x = box.right() + 14;
    }
    const QString status = m_playing ? QStringLiteral("PLAYING") : QStringLiteral("ONLINE");
    const QRectF badge(168, 29, p.fontMetrics().horizontalAdvance(status) + 16, 14);
    p.setPen(Qt::NoPen);
    p.setBrush(m_playing ? Theme::lcdGlow : QColor(Theme::lcdGlow.red(), Theme::lcdGlow.green(), Theme::lcdGlow.blue(), 170));
    p.drawRoundedRect(badge, 3, 3);
    p.setPen(Theme::lcdBg);
    p.drawText(badge, Qt::AlignCenter, status);

    // spectrum analyzer
    const qreal barW = 7, barGap = 3, vizH = 34;
    const qreal vizX = width() - 14 - Bands * (barW + barGap);
    for (int i = 0; i < Bands; ++i) {
        const qreal bx = vizX + i * (barW + barGap);
        const int lit = int(m_bands[i] * 11);
        for (int s = 0; s < 11; ++s) {
            const qreal by = 10 + vizH - (s + 1) * 3.1;
            p.fillRect(QRectF(bx, by, barW, 2), s < lit ? Theme::lcdGlow : Theme::lcdDim);
        }
        const qreal py = 10 + vizH - (int(m_peaks[i] * 11) + 1) * 3.1;
        p.fillRect(QRectF(bx, py, barW, 1.5), QColor(0xc8, 0xe0, 0xff));
    }

    // dot-matrix marquee
    const QString line = QStringLiteral("PDA :: %1  -  %2").arg(m_page.toUpper(), m_message.toUpper());
    Metal::dotMatrix(p, QRectF(12, 52, width() - 24, 30), line, m_scroll, Theme::lcdGlow, Theme::lcdDim);
}
