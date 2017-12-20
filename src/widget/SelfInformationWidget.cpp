/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "SelfInformationWidget.h"

#include "AppSettings.h"
#include "ui_SelfInformationWidget.h"

#include <QDesktopServices>
#include <QPainter>
#include <QUrl>

using namespace governikus;

SelfInformationWidget::SelfInformationWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::SelfInformationWidget())
{
	mUi->setupUi(this);

	connect(mUi->selfAuthenticationButton, &QAbstractButton::clicked, this, &SelfInformationWidget::selfAuthenticationRequested);
	connect(&AppSettings::getInstance().getGeneralSettings(), &AbstractSettings::fireSettingsChanged, this, &SelfInformationWidget::onSettingsChanged);

	mPixDescLogoLabel.reset(new QPixmap(QStringLiteral(":/images/siteWithLogo.png")));
	mUi->descriptionLogoLabel->setPixmap(mPixDescLogoLabel->scaled(159, 120, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

	onSettingsChanged();
}


SelfInformationWidget::~SelfInformationWidget()
{
}


void SelfInformationWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void SelfInformationWidget::onSettingsChanged()
{
	if (AppSettings::getInstance().getGeneralSettings().useSelfAuthTestUri())
	{
		mUi->selfAuthenticationButton->setStyleSheet(QStringLiteral("QPushButton { background: red; }"));
		mUi->selfAuthenticationButton->setToolTip(tr("Test environment"));
	}
	else
	{
		mUi->selfAuthenticationButton->setStyleSheet(QString());
		mUi->selfAuthenticationButton->setToolTip(QString());
	}
}


void SelfInformationWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
