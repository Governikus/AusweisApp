/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "BusyOverlay.h"
#include "ui_BusyOverlay.h"

#include <QGraphicsOpacityEffect>
#include <QMovie>
#include <QPainter>
#include <QSizePolicy>
#include <QStackedLayout>

using namespace governikus;

BusyOverlay::BusyOverlay(bool pStart, QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::BusyOverlay())
	, mMovie(new QMovie(QStringLiteral(":/images/busy_animation.gif")))
{
	mUi->setupUi(this);

	mUi->busyAnimationLabel->setMovie(mMovie.data());

	setStyleSheet(QStringLiteral("background-color: white;"));

	QGraphicsOpacityEffect* opacity = new QGraphicsOpacityEffect(this);
	opacity->setOpacity(0.9);
	setGraphicsEffect(opacity);

	if (pStart)
	{
		mMovie->start();
	}
}


BusyOverlay::~BusyOverlay()
{
}


void BusyOverlay::startAnimation()
{
	if (mMovie->state() != QMovie::Running)
	{
		mMovie->start();
	}
}


void BusyOverlay::stopAnimation()
{
	if (mMovie->state() == QMovie::Running)
	{
		mMovie->stop();
	}
}


QSize BusyOverlay::sizeHint() const
{
	// make square
	QSize hint = QWidget::sizeHint();
	int result = std::max(hint.width(), hint.height());
	return QSize(result, result);
}


void BusyOverlay::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void BusyOverlay::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
