/*!
 * AuthenticateStepsWidget.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AuthenticateStepsWidget.h"
#include "ui_AuthenticateStepsWidget.h"

#include <QPainter>

#include "AppStartPage.h"
#include "generic/BusyOverlayContainer.h"

using namespace governikus;

AuthenticateStepsWidget::AuthenticateStepsWidget(QWidget* pParent)
	: QStackedWidget(pParent)
	, mUi(new Ui::AuthenticateStepsWidget())
	, mProcessingPage(new BusyOverlayContainer(new AppStartPage(), false))
{
	mUi->setupUi(this);

	addWidget(mProcessingPage);
}


AuthenticateStepsWidget::~AuthenticateStepsWidget()
{
}


StepAuthenticationEac1Widget* AuthenticateStepsWidget::getEac1Page() const
{
	return mUi->authenticationEac1Page;
}


SelfInfoWidget* AuthenticateStepsWidget::getSelfInfoPage() const
{
	return mUi->selfInfoPage;
}


void AuthenticateStepsWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
