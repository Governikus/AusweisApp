/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "HistoryDetailWidget.h"
#include "ui_HistoryDetailWidget.h"

#include <QPainter>

using namespace governikus;


HistoryDetailWidget::HistoryDetailWidget(QWidget* pParent)
	: Page(tr("History details"), pParent)
	, mUi(new Ui::HistoryDetailWidget())
{
	mUi->setupUi(this);
}


HistoryDetailWidget::~HistoryDetailWidget()
{
}


void HistoryDetailWidget::setDetails(const QString& pDetails)
{
	mUi->detailText->setText(pDetails);
}


void HistoryDetailWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
