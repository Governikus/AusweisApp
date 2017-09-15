/*!
 * AppStartPage.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AppStartPage.h"

#include "generic/LayoutBuilder.h"
#include "generic/TabButtonGroup.h"
#include "GuiProfile.h"
#include "ui_AppStartPage.h"

#include <QPainter>
#include <QStyleOption>

using namespace governikus;

AppStartPage::AppStartPage(QWidget* pParent)
	: Page(QStringLiteral(" "), pParent)
	, mApplicationLogoLabel(new QLabel(this))
	, mUi(new Ui::AppStartPage())
{
	mUi->setupUi(this);

	QPixmap applicationPixmap(QStringLiteral(":/images/AppLogo_AutentApp2_2014.png"));
	QPixmap nPAeATPixmap(QStringLiteral(":/images/start_nPA_eAT.png"));

	if (GuiProfile::getProfile().isSmallScreen())
	{
		mUi->applicationLogoLabel->setPixmap(applicationPixmap.scaledToWidth(186, Qt::SmoothTransformation));
		mUi->nPAeATLabel->setPixmap(nPAeATPixmap.scaledToWidth(186, Qt::SmoothTransformation));
	}
	else
	{
		// assume we have enough space
		mUi->applicationLogoLabel->setPixmap(applicationPixmap);
		mUi->nPAeATLabel->setPixmap(nPAeATPixmap);
	}
}


AppStartPage::~AppStartPage()
{
}


void AppStartPage::paintEvent(QPaintEvent* /*pPaintEvent*/)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
