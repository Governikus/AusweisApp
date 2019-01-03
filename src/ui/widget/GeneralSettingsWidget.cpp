/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "GeneralSettingsWidget.h"
#include "ui_GeneralSettingsWidget.h"

#include "AppSettings.h"
#include "Service.h"
#include "UpdateWindow.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(settings)


GeneralSettingsWidget::GeneralSettingsWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::GeneralSettingsWidget())
{
	mUi->setupUi(this);

	auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	if (generalSettings.autoStartIsSetByAdmin())
	{
		qCInfo(settings) << "Auto start is enabled system wide";
		mUi->autostartCheckBox->setEnabled(false);
	}
	if (generalSettings.autoUpdateCheckIsSetByAdmin())
	{
		qCInfo(settings) << "Auto update check is configured system wide";
		mUi->regularlyUpdateCheckBox->setEnabled(false);
	}

	reset();

	connect(mUi->autostartCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
	connect(mUi->regularlyUpdateCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
	connect(mUi->closeWindowCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
	connect(mUi->saveHistoryCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
	connect(mUi->updateCheckButton, &QCheckBox::clicked, this, &GeneralSettingsWidget::onUpdateCheckButtonClicked);
	connect(mUi->keylessPasswordCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
}


GeneralSettingsWidget::~GeneralSettingsWidget()
{
}


void GeneralSettingsWidget::showEvent(QShowEvent* pEvent)
{
	disconnect(mUi->saveHistoryCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);
	mUi->saveHistoryCheckBox->setChecked(Env::getSingleton<AppSettings>()->getHistorySettings().isEnabled());
	connect(mUi->saveHistoryCheckBox, &QCheckBox::stateChanged, this, &GeneralSettingsWidget::onCheckBoxStateChanged);

	QWidget::showEvent(pEvent);
}


void GeneralSettingsWidget::apply()
{
	auto& historySettings = Env::getSingleton<AppSettings>()->getHistorySettings();
	historySettings.setEnabled(mUi->saveHistoryCheckBox->isChecked());
	historySettings.save();

	auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	generalSettings.setAutoCloseWindowAfterAuthentication(mUi->closeWindowCheckBox->isChecked());
	generalSettings.setAutoStart(mUi->autostartCheckBox->isChecked());
	generalSettings.setAutoUpdateCheck(mUi->regularlyUpdateCheckBox->isChecked());
	generalSettings.setUseScreenKeyboard(mUi->keylessPasswordCheckBox->isChecked());
	generalSettings.save();
}


void GeneralSettingsWidget::reset()
{
	const auto& historySettings = Env::getSingleton<AppSettings>()->getHistorySettings();
	mUi->saveHistoryCheckBox->setChecked(historySettings.isEnabled());

	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
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
	Env::getSingleton<Service>()->updateApp(true);
}


void GeneralSettingsWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
