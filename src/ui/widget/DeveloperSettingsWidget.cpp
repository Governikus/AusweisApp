/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "DeveloperSettingsWidget.h"
#include "ui_DeveloperSettingsWidget.h"

#include "AppSettings.h"

#include <QSignalBlocker>

using namespace governikus;

DeveloperSettingsWidget::DeveloperSettingsWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::DeveloperSettingsWidget())
{
	mUi->setupUi(this);

	mUi->lblDeveloperModeWarning->setText(QStringLiteral("<html><head/><body><p><span style='color:red;'>%1</span></p></body></html>")
			.arg(tr("The developer mode is aimed at integrators / developers for new service applications."
					" For this reason, the developer mode works only in the test PKI."
					" By activating the developer mode, some safety tests are deactivated."
					" This means that the authentication process continues although the AusweisApp2 would usually abort the process with an error message when used in normal operation mode."
					" Information on the disregarded error in the developer mode is displayed in the attached window below the AusweisApp2.")));

	connect(mUi->selfAuthTestCheckBox, &QCheckBox::stateChanged, this, &DeveloperSettingsWidget::onCheckBoxStateChanged);
	connect(mUi->chkbDeveloperMode, &QCheckBox::stateChanged, this, &DeveloperSettingsWidget::onCheckBoxStateChanged);

	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(&generalSettings, &GeneralSettings::fireDeveloperOptionsChanged, this, &DeveloperSettingsWidget::reset);

	reset();
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
	generalSettings.setDeveloperMode(mUi->chkbDeveloperMode->isChecked());
	generalSettings.save();
}


void DeveloperSettingsWidget::reset()
{
	// Avoid the fireSettingsChanged signal in onCheckBoxStateChanged when the
	// developer mode is disabled by the button outside of the settings view.
	const QSignalBlocker blocker(this);
	mUi->selfAuthTestCheckBox->setChecked(Env::getSingleton<AppSettings>()->getGeneralSettings().useSelfAuthTestUri());
	mUi->chkbDeveloperMode->setChecked(Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode());
}


void DeveloperSettingsWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
