#include <QPainter>

#include <tweeny.h>

#include "SnackBar.h"

constexpr int STARTING_OFFSET         = 1;
constexpr int ANIMATION_DURATION      = 6'000;
constexpr int BOX_PADDING             = 10;
constexpr double MIN_WIDTH            = 400.0;
constexpr double MIN_WIDTH_PERCENTAGE = 0.3;

SnackBar::SnackBar(QWidget *parent)
  : OverlayWidget(parent)
{
        QFont font;
        font.setPointSizeF(font.pointSizeF() * 1.2);
        font.setWeight(50);
        setFont(font);

        boxHeight_ = QFontMetrics(font).height() * 2;
        offset_    = STARTING_OFFSET;
        position_  = SnackBarPosition::Top;

        hideTimer_.setSingleShot(true);

        auto offset_anim = tweeny::from(1.0f).to(0.0f).during(100).via(tweeny::easing::cubicOut);
        connect(&showTimer_, &QTimer::timeout, this, [this, offset_anim]() mutable {
                if (offset_anim.progress() < 1.0f) {
                        offset_ = offset_anim.step(0.07f);
                        repaint();
                } else {
                        showTimer_.stop();
                        hideTimer_.start(ANIMATION_DURATION);
                        offset_anim.seek(0.0f);
                }
        });

        connect(&hideTimer_, SIGNAL(timeout()), this, SLOT(hideMessage()));

        hide();
}

void
SnackBar::start()
{
        if (messages_.empty())
                return;

        show();
        raise();

        showTimer_.start(10);
}

void
SnackBar::hideMessage()
{
        stopTimers();
        hide();

        if (!messages_.empty())
                // Moving on to the next message.
                messages_.pop_front();

        // Reseting the starting position of the widget.
        offset_ = STARTING_OFFSET;

        if (!messages_.empty())
                start();
}

void
SnackBar::stopTimers()
{
        showTimer_.stop();
        hideTimer_.stop();
}

void
SnackBar::showMessage(const QString &msg)
{
        messages_.push_back(msg);

        // There is already an active message.
        if (isVisible())
                return;

        start();
}

void
SnackBar::mousePressEvent(QMouseEvent *)
{
        hideMessage();
}

void
SnackBar::paintEvent(QPaintEvent *event)
{
        Q_UNUSED(event)

        if (messages_.empty())
                return;

        auto message_ = messages_.front();

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(bgColor_);
        p.setBrush(brush);

        QRect r(0, 0, std::max(MIN_WIDTH, width() * MIN_WIDTH_PERCENTAGE), boxHeight_);

        p.setPen(Qt::white);
        QRect br = p.boundingRect(r, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, message_);

        p.setPen(Qt::NoPen);
        r = br.united(r).adjusted(-BOX_PADDING, -BOX_PADDING, BOX_PADDING, BOX_PADDING);

        const qreal s = 1 - offset_;

        if (position_ == SnackBarPosition::Bottom)
                p.translate((width() - (r.width() - 2 * BOX_PADDING)) / 2,
                            height() - BOX_PADDING - s * (r.height()));
        else
                p.translate((width() - (r.width() - 2 * BOX_PADDING)) / 2,
                            s * (r.height()) - 2 * BOX_PADDING);

        br.moveCenter(r.center());
        p.drawRoundedRect(r.adjusted(0, 0, 0, 4), 4, 4);
        p.setPen(textColor_);
        p.drawText(br, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, message_);
}
