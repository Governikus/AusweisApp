/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "SelfInformationWidget.h"

#include "ui_SelfInformationWidget.h"

#include <QDesktopServices>
#include <QPainter>
#include <QUrl>

using namespace governikus;

SelfInformationWidget::SelfInformationWidget(QWidget* pParent)
	: Page(tr("Identify"), pParent)
	, mUi(new Ui::SelfInformationWidget())
{
	mUi->setupUi(this);

	connect(mUi->selfAuthenticationButton, &QAbstractButton::clicked, this, &SelfInformationWidget::selfAuthenticationRequested);

	mPixDescLogoLabel.reset(new QPixmap(QStringLiteral(":/images/siteWithLogo.png")));
	mUi->descriptionLogoLabel->setPixmap(mPixDescLogoLabel->scaled(159, 120, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
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
