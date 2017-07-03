/*!
 * SettingsWidget.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"
#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"

#include <QMessageBox>
#include <QMetaObject>
#include <QPainter>
#include <QPushButton>

using namespace governikus;

SettingsWidget::SettingsWidget(QWidget* pParent)
	: Page(tr("Settings"), pParent)
	, mUi(new Ui::SettingsWidget())
	, mSettingsChanged(false)
{
	mUi->setupUi(this);

	mUi->driverButton->setHidden(true);

	connect(mUi->diagnosisButton, &QAbstractButton::clicked, this, &SettingsWidget::diagnosisRequested);
	connect(mUi->driverButton, &QAbstractButton::clicked, this, &SettingsWidget::driverRequested);

	connect(mUi->generalTab, &GeneralSettingsWidget::settingsChanged, this, &SettingsWidget::onSettingsChanged);
	connect(mUi->pinTab, &PinSettingsWidget::setChangePinButtonEnabled, this, &SettingsWidget::onSetChangePinButtonEnabled);

	connect(mUi->cancelButton, &QPushButton::clicked, this, &SettingsWidget::onCancelButtonClicked);

	mUi->applyButton->setText(tr("Apply"));
	mApplyButtonText = mUi->applyButton->text();
	connect(mUi->applyButton, &QPushButton::clicked, this, &SettingsWidget::onApplyButtonClicked);

	connect(mUi->settingsTabWidget, &QTabWidget::currentChanged, this, &SettingsWidget::onTabChanged);

	mUi->pinTab->setUseScreenKeyboard(AppSettings::getInstance().getGeneralSettings().isUseScreenKeyboard());

	setSettingsChanged(false);
}


SettingsWidget::~SettingsWidget()
{
}


void SettingsWidget::workflowStarted()
{
	// disable the non-selected tabs
	int tabCount = mUi->settingsTabWidget->count();
	for (int i = 0; i < tabCount; ++i)
	{
		mUi->settingsTabWidget->setTabEnabled(i, i == mUi->settingsTabWidget->currentIndex());
	}

	// disable buttons
	mUi->applyButton->setEnabled(false);
	mUi->cancelButton->setEnabled(false);
}


void SettingsWidget::workflowFinished()
{
	// enable all tabs
	int tabCount = mUi->settingsTabWidget->count();
	for (int i = 0; i < tabCount; ++i)
	{
		mUi->settingsTabWidget->setTabEnabled(i, true);
	}

	// enable buttons
	mUi->applyButton->setEnabled(mSettingsChanged);
	mUi->cancelButton->setEnabled(true);

	if (mUi->pinTab->isVisible())
	{
		QMetaObject::invokeMethod(mUi->pinTab, "updateReaders", Qt::QueuedConnection);
	}
}


void SettingsWidget::switchToGuiModule(GuiModule pModule)
{
	switch (pModule)
	{
		case GuiModule::START_PAGE:
			// not handled here
			break;

		case GuiModule::GENERAL_SETTINGS:
			mUi->settingsTabWidget->setCurrentWidget(mUi->generalTab);
			break;

		case GuiModule::PIN_SETTINGS:
			mUi->settingsTabWidget->setCurrentWidget(mUi->pinTab);
			break;
	}
}


void SettingsWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void SettingsWidget::hideEvent(QHideEvent* pEvent)
{
	onTabChanged(-1);
	Page::hideEvent(pEvent);
}


void SettingsWidget::onTabChanged(int pIndex)
{
	if (mUi->settingsTabWidget->widget(pIndex) == mUi->pinTab || pIndex == -1)
	{
		if (mSettingsChanged)
		{
			showSettingsChangedMessage();
		}
		mUi->pinTab->setUseScreenKeyboard(AppSettings::getInstance().getGeneralSettings().isUseScreenKeyboard());
	}
	else
	{
		mUi->applyButton->setText(mApplyButtonText);
		setSettingsChanged(mSettingsChanged);
	}
}


void SettingsWidget::showSettingsChangedMessage()
{
	QMessageBox msgBox(this);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setWindowTitle(QCoreApplication::applicationName() + " - " + tr("Apply settings?"));
	msgBox.setWindowModality(Qt::WindowModal);
	msgBox.setText(tr("Do you want to apply the changes?"));

	if (msgBox.exec() == QMessageBox::Yes)
	{
		applyAppSettings();
	}
	else
	{
		resetSettings();
	}
}


void SettingsWidget::onApplyButtonClicked()
{
	if (mUi->settingsTabWidget->currentWidget() == mUi->pinTab)
	{
		// change PIN button clicked
		if (mUi->pinTab->getMode() == PinSettingsWidget::Mode::AfterPinChange)
		{
			mUi->pinTab->setMode(PinSettingsWidget::Mode::Normal);
			mUi->pinTab->updateReaders();
		}
		else
		{
			if (mUi->pinTab->isVisible())
			{
				Q_EMIT changePinRequested();
			}
		}
	}
	else
	{
		applyAppSettings();
	}
}


bool SettingsWidget::isSettingsChanged()
{
	return mSettingsChanged;
}


void SettingsWidget::applyAppSettings()
{
	// apply button clicked
	mUi->generalTab->apply();

	setSettingsChanged(false);
}


void SettingsWidget::onCancelButtonClicked()
{
	resetSettings();
	Q_EMIT settingsDone();
}


void SettingsWidget::onSetChangePinButtonEnabled(bool pEnabled)
{
	if (mUi->settingsTabWidget->currentWidget() == mUi->pinTab)
	{
		mUi->applyButton->setText(mUi->pinTab->getButtonText());
		mUi->applyButton->setEnabled(pEnabled);
		if (pEnabled)
		{
			mUi->applyButton->setAutoDefault(true);
			mUi->applyButton->setDefault(true);
			mUi->applyButton->setFocus();
		}
	}
}


void SettingsWidget::onSettingsChanged()
{
	setSettingsChanged(true);
}


void SettingsWidget::resetSettings()
{
	mUi->generalTab->reset();
	setSettingsChanged(false);
}


QString SettingsWidget::getActiveTabObjectName()
{
	return mUi->settingsTabWidget->currentWidget()->objectName();
}


void SettingsWidget::setSettingsChanged(bool pChanged)
{
	mSettingsChanged = pChanged;
	mUi->applyButton->setEnabled(pChanged);
}
