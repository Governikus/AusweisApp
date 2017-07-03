/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "PinSettingsInfoWidget.h"
#include "ui_PinSettingsInfoWidget.h"

#include <QPainter>

using namespace governikus;


PinSettingsInfoWidget::PinSettingsInfoWidget(QWidget* pParent)
	: Page(tr("Information"), pParent)
	, mUi(new Ui::PinSettingsInfoWidget())
{
	mUi->setupUi(this);
}


PinSettingsInfoWidget::~PinSettingsInfoWidget()
{
}


void PinSettingsInfoWidget::setInfoTitle(const QString& pTitle)
{
	mUi->infoTitle->setText(pTitle);
}


void PinSettingsInfoWidget::setInfoDescription(const QString& pDescription)
{
	mUi->infoDescription->setText(pDescription);
}


void PinSettingsInfoWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
