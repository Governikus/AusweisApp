/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "GeneralSettingsWidget.h"

#include "AppSettings.h"
#include "UpdateWindow.h"
#include "ui_GeneralSettingsWidget.h"

using namespace governikus;

GeneralSettingsWidget::GeneralSettingsWidget(QWidget* pParent)
	: Page(tr("General"), pParent)
	, mUi(new Ui::GeneralSettingsWidget())
{
	mUi->setupUi(this);

	reset();

	connect(mUi->autostartCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
	connect(mUi->regularlyUpdateCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
	connect(mUi->closeWindowCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
	connect(mUi->safeHistoryCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
	connect(mUi->updateCheckButton, &QCheckBox::clicked, this, &GeneralSettingsWidget::onUpdateCheckButtonClicked);
	connect(mUi->keylessPasswordCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
}


GeneralSettingsWidget::~GeneralSettingsWidget()
{
}


void GeneralSettingsWidget::showEvent(QShowEvent* pEvent)
{
	disconnect(mUi->safeHistoryCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
	mUi->safeHistoryCheckBox->setChecked(AppSettings::getInstance().getHistorySettings().isEnabled());
	connect(mUi->safeHistoryCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);

	QWidget::showEvent(pEvent);
}


void GeneralSettingsWidget::apply()
{
	AppSettings::getInstance().getHistorySettings().setEnabled(mUi->safeHistoryCheckBox->isChecked());
	AppSettings::getInstance().getHistorySettings().save();

	auto& generalSettings = AppSettings::getInstance().getGeneralSettings();
	generalSettings.setAutoCloseWindowAfterAuthentication(mUi->closeWindowCheckBox->isChecked());
	generalSettings.setAutoStart(mUi->autostartCheckBox->isChecked());
	generalSettings.setAutoUpdateCheck(mUi->regularlyUpdateCheckBox->isChecked());
	generalSettings.setUseScreenKeyboard(mUi->keylessPasswordCheckBox->isChecked());
	generalSettings.save();
}


void GeneralSettingsWidget::reset()
{
	mUi->safeHistoryCheckBox->setChecked(AppSettings::getInstance().getHistorySettings().isEnabled());

	const auto& generalSettings = AppSettings::getInstance().getGeneralSettings();
	mUi->closeWindowCheckBox->setChecked(generalSettings.isAutoCloseWindowAfterAuthentication());
	mUi->autostartCheckBox->setChecked(generalSettings.isAutoStart());
	mUi->regularlyUpdateCheckBox->setChecked(generalSettings.isAutoUpdateCheck());
	mUi->keylessPasswordCheckBox->setChecked(generalSettings.isUseScreenKeyboard());
}


void GeneralSettingsWidget::onCheckBoxStateChanged(int)
{
	Q_EMIT settingsChanged();
}


void GeneralSettingsWidget::onUpdateCheckButtonClicked()
{
	new UpdateWindow(false, this);
}
