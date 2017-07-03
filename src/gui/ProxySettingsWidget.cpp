/*!
 * ProxySettingsWidget.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ProxySettingsWidget.h"

#include "ui_ProxySettingsWidget.h"

#include <QPainter>
#include <QSettings>

using namespace governikus;

ProxySettingsWidget::ProxySettingsWidget(QWidget* pParent)
	: Page(tr("Extended"), pParent)
	, mUi(new Ui::ProxySettingsWidget())
{
	mUi->setupUi(this);

	setContentsMargins(9, 9, 9, 9);
	connect(mUi->noProxyRadioButton, &QRadioButton::toggled, this, &ProxySettingsWidget::onProxyTypeChanged);
	connect(mUi->systemProxyRadioButton, &QRadioButton::toggled, this, &ProxySettingsWidget::onProxyTypeChanged);
}


ProxySettingsWidget::~ProxySettingsWidget()
{
}


/*
   ProxyType ProxySettingsWidget::getProxyType() const
   {
    if (mUi->systemProxyRadioButton->isChecked())
    {
        return ProxyType::SYSTEM;
    }
    return ProxyType::NONE;
   }


   void ProxySettingsWidget::setProxyType(ProxyType pType)
   {
    switch (pType)
    {
        case ProxyType::NONE:
            mUi->noProxyRadioButton->setChecked(true);
            break;

        case ProxyType::SYSTEM:
            mUi->systemProxyRadioButton->setChecked(true);
            break;
    }
   }
 */
void ProxySettingsWidget::onProxyTypeChanged(bool pChecked)
{
	if (pChecked)
	{
		return;
	}
	onProxySettingsChanged();
}


void ProxySettingsWidget::onProxySettingsChanged()
{
	Q_EMIT settingsChanged();
}


void ProxySettingsWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
