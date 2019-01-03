/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "AppStartPage.h"
#include "ui_AppStartPage.h"

#include "generic/TabButtonGroup.h"

#include <QPainter>
#include <QStyleOption>

using namespace governikus;

AppStartPage::AppStartPage(QWidget* pParent)
	: QWidget(pParent)
	, mApplicationLogoLabel(new QLabel(this))
	, mUi(new Ui::AppStartPage())
{
	mUi->setupUi(this);

	QPixmap applicationPixmap(QStringLiteral(":/images/Logo_AusweisApp2.png"));
	QPixmap nPAeATPixmap(QStringLiteral(":/images/start_nPA_eAT.png"));

	mUi->applicationLogoLabel->setPixmap(applicationPixmap);
	mUi->nPAeATLabel->setPixmap(nPAeATPixmap);
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


void AppStartPage::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
