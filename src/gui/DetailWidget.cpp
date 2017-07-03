/*!
 * DetailWidget.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "DetailWidget.h"
#include "ui_DetailWidget.h"
#include <QPainter>

using namespace governikus;

DetailWidget::DetailWidget(QWidget* pParent)
	: Page(tr("Details"), pParent)
	, mUi(new Ui::DetailWidget())
{
	mUi->setupUi(this);

	// The scroll area may be resized while the content keeps its
	// size, therefore the scroll area must have the same background
	// color as the content.
	mUi->scrollArea->setStyleSheet("QScrollArea { background-color: white; }"
								   ""
								   "QScrollBar {"
								   "  background-color: #f8f6f4;"
								   "}");
	mUi->scrollAreaWidgetContents->setStyleSheet("border-width: 0;");
	mUi->detailText->setStyleSheet("border-width: 0;");
}


DetailWidget::~DetailWidget()
{
}


void DetailWidget::setDetails(const QString& pDetails)
{
	mUi->detailText->setAccessibleName(tr("Service provider details dialog") + pDetails);
	mUi->detailText->setText(pDetails);
}


void DetailWidget::paintEvent(QPaintEvent*)
{
	static const int SCROLLAREA_PREFERRED_WIDTH = 504;
	// See comment to accepted answer at
	// http://stackoverflow.com/questions/16515646/how-to-get-scroll-bar-real-width-in-qt
	static const int SCROLLBAR_EXTRA_SPACE = 3;

	const int scrollbarWidth = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent) + SCROLLBAR_EXTRA_SPACE;
	const int contentMaxWidth = SCROLLAREA_PREFERRED_WIDTH - scrollbarWidth;

	if (mUi->detailText->width() > contentMaxWidth)
	{
		mUi->detailText->setFixedWidth(contentMaxWidth);
	}

	mUi->scrollAreaWidgetContents->setFixedHeight(mUi->detailText->height());
	mUi->scrollAreaWidgetContents->setFixedWidth(mUi->detailText->width());

	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
