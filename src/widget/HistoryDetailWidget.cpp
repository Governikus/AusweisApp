/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "HistoryDetailWidget.h"

#include "ui_HistoryDetailWidget.h"

#include <QPainter>
#include <QStyleOption>

using namespace governikus;


HistoryDetailWidget::HistoryDetailWidget(QWidget* pParent)
	: QWidget(pParent)
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


void HistoryDetailWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
