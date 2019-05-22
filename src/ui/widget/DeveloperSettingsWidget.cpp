/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DeveloperSettingsWidget.h"
#include "ui_DeveloperSettingsWidget.h"

#include "AppSettings.h"

using namespace governikus;

DeveloperSettingsWidget::DeveloperSettingsWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::DeveloperSettingsWidget())
{
	mUi->setupUi(this);

	reset();

	connect(mUi->selfAuthTestCheckBox, &QCheckBox::stateChanged, this, &DeveloperSettingsWidget::onCheckBoxStateChanged);
}


DeveloperSettingsWidget::~DeveloperSettingsWidget()
{
}


void governikus::DeveloperSettingsWidget::onCheckBoxStateChanged()
{
	Q_EMIT fireSettingsChanged();
}


void DeveloperSettingsWidget::showEvent(QShowEvent* pEvent)
{
	QWidget::showEvent(pEvent);
}


void DeveloperSettingsWidget::apply()
{
	auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	generalSettings.setUseSelfauthenticationTestUri(mUi->selfAuthTestCheckBox->isChecked());
	generalSettings.save();
}


void DeveloperSettingsWidget::reset()
{
	mUi->selfAuthTestCheckBox->setChecked(Env::getSingleton<AppSettings>()->getGeneralSettings().useSelfAuthTestUri());
}


void DeveloperSettingsWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
