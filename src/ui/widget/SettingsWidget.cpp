/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"

#include "AppSettings.h"
#include "ReaderManager.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QMetaObject>
#include <QPainter>
#include <QPushButton>

using namespace governikus;

SettingsWidget::SettingsWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::SettingsWidget())
	, mScanRunning(false)
	, mWorkflowRunning(false)
	, mSettingsChanged(false)
{
	mUi->setupUi(this);

	connect(mUi->diagnosisButton, &QAbstractButton::clicked, this, &SettingsWidget::diagnosisRequested);

	connect(mUi->generalTab, &GeneralSettingsWidget::settingsChanged, this, &SettingsWidget::onSettingsChanged);
	connect(mUi->generalTab, &GeneralSettingsWidget::fireSwitchUiRequested, this, &SettingsWidget::fireSwitchUiRequested);
	connect(mUi->pinTab, &PinSettingsWidget::fireButtonEnabledUpdated, this, &SettingsWidget::onUpdateButtonState);
	connect(mUi->pinTab, &PinSettingsWidget::fireButtonEnabledUpdated, this, &SettingsWidget::onUpdateApplyButtonText);

	connect(mUi->cancelButton, &QPushButton::clicked, this, &SettingsWidget::onCancelButtonClicked);
	connect(mUi->applyButton, &QPushButton::clicked, this, &SettingsWidget::onApplyButtonClicked);

	connect(mUi->settingsTabWidget, &QTabWidget::currentChanged, this, &SettingsWidget::onTabChanged);

	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	mUi->pinTab->setUseScreenKeyboard(generalSettings.isUseScreenKeyboard());
	connect(this, &SettingsWidget::fireBackspacePressedOnApply, mUi->pinTab, &PinSettingsWidget::onBackspacePressedOnApply);

	mDeveloperTab.reset(new DeveloperSettingsWidget());
	mDeveloperTab->setObjectName(QStringLiteral("developerTab"));
	connect(mDeveloperTab.data(), &DeveloperSettingsWidget::fireSettingsChanged, this, &SettingsWidget::onSettingsChanged);
	connect(&generalSettings, &GeneralSettings::fireDeveloperOptionsChanged, this, &SettingsWidget::onDeveloperOptionsChanged);

	setSettingsChanged(false);
	onUpdateApplyButtonText();
	onDeveloperOptionsChanged();
}


SettingsWidget::~SettingsWidget()
{
}


void SettingsWidget::keyPressEvent(QKeyEvent* pEvent)
{
	if (pEvent->key() == Qt::Key_Backspace && mUi->applyButton->hasFocus())
	{
		Q_EMIT fireBackspacePressedOnApply();
	}
	QWidget::keyPressEvent(pEvent);
}


void SettingsWidget::workflowStarted()
{
	mWorkflowRunning = true;

	// disable the non-selected tabs
	int tabCount = mUi->settingsTabWidget->count();
	for (int i = 0; i < tabCount; ++i)
	{
		mUi->settingsTabWidget->setTabEnabled(i, i == mUi->settingsTabWidget->currentIndex());
	}

	onUpdateButtonState();
}


void SettingsWidget::workflowFinished()
{
	mWorkflowRunning = false;

	// enable all tabs
	int tabCount = mUi->settingsTabWidget->count();
	for (int i = 0; i < tabCount; ++i)
	{
		mUi->settingsTabWidget->setTabEnabled(i, true);
	}

	onUpdateButtonState();

	if (mUi->pinTab->isVisible())
	{
		QMetaObject::invokeMethod(mUi->pinTab, &PinSettingsWidget::updateReaders, Qt::QueuedConnection);
	}
}


void SettingsWidget::switchToGuiModule(GuiModule pModule)
{
	switch (pModule)
	{
		case GuiModule::START_PAGE:
		case GuiModule::IDENTIFY:
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
	QWidget::hideEvent(pEvent);
}


void SettingsWidget::showEvent(QShowEvent* pEvent)
{
	QWidget::showEvent(pEvent);

	if (mUi->settingsTabWidget->currentWidget() == mUi->pinTab || mUi->settingsTabWidget->currentWidget() == mUi->readerDeviceTab)
	{
		mScanRunning = true;
		Env::getSingleton<ReaderManager>()->startScanAll(mUi->settingsTabWidget->currentWidget() == mUi->pinTab);
	}
}


void SettingsWidget::onTabChanged(int pIndex)
{
	QWidget* const currentWidget = mUi->settingsTabWidget->widget(pIndex);
	if (currentWidget != mUi->generalTab && currentWidget != mDeveloperTab.data())
	{
		if (mSettingsChanged)
		{
			showSettingsChangedMessage();
		}
		mUi->pinTab->setUseScreenKeyboard(Env::getSingleton<AppSettings>()->getGeneralSettings().isUseScreenKeyboard());
	}
	else
	{
		setSettingsChanged(mSettingsChanged);
	}

	const auto readerManager = Env::getSingleton<ReaderManager>();
	if (currentWidget == mUi->pinTab || currentWidget == mUi->readerDeviceTab)
	{
		mScanRunning = true;
		readerManager->startScanAll(currentWidget == mUi->pinTab);
	}
	else if (mScanRunning)
	{
		mScanRunning = false;
		readerManager->stopScanAll();
	}

	onUpdateApplyButtonText();
	onUpdateButtonState();
}


void SettingsWidget::showSettingsChangedMessage()
{
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Apply settings?"));
	msgBox.setWindowModality(Qt::WindowModal);
	msgBox.setText(tr("Do you want to apply the changes?"));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.button(QMessageBox::Yes)->setFocus();

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
	const QWidget* const currentWidget = mUi->settingsTabWidget->currentWidget();
	if (currentWidget == mUi->pinTab)
	{
		if (mWorkflowRunning)
		{
			mUi->pinTab->continueWorkflow();
		}
		else if (mUi->pinTab->getMode() == PinSettingsWidget::Mode::AfterPinChange)
		{
			mUi->pinTab->setMode(PinSettingsWidget::Mode::Normal);
			mUi->pinTab->updateReaders();
		}
		else if (mUi->pinTab->isVisible())
		{
			Q_EMIT changePinRequested();
		}
	}
	else if (currentWidget == mUi->generalTab || currentWidget == mDeveloperTab.data())
	{
		applyAppSettings();
	}
	else if (currentWidget == mUi->readerDeviceTab)
	{
		mUi->settingsTabWidget->setCurrentWidget(mUi->generalTab);
	}
}


bool SettingsWidget::isSettingsChanged()
{
	return mSettingsChanged;
}


void SettingsWidget::applyAppSettings()
{
	mUi->generalTab->apply();
	mDeveloperTab->apply();
	setSettingsChanged(false);
}


void SettingsWidget::onCancelButtonClicked()
{
	if (mWorkflowRunning)
	{
		mUi->pinTab->cancelWorkflow();
	}

	resetSettings();
	Q_EMIT settingsDone();
}


void SettingsWidget::onSettingsChanged()
{
	setSettingsChanged(true);
}


void SettingsWidget::onDeveloperOptionsChanged()
{
	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	const int index = mUi->settingsTabWidget->indexOf(mDeveloperTab.data());

	if (generalSettings.isDeveloperOptions() && index == -1)
	{
		mUi->settingsTabWidget->addTab(mDeveloperTab.data(), QString());
		setDeveloperTabName();
		return;
	}

	if (!generalSettings.isDeveloperOptions() && index > -1)
	{
		if (index == mUi->settingsTabWidget->currentIndex())
		{
			mSettingsChanged = false;
		}
		mUi->settingsTabWidget->removeTab(index);
	}
}


void SettingsWidget::resetSettings()
{
	mUi->generalTab->reset();
	mDeveloperTab->reset();
	setSettingsChanged(false);
}


QString SettingsWidget::getActiveTabObjectName()
{
	return mUi->settingsTabWidget->currentWidget()->objectName();
}


void SettingsWidget::setSettingsChanged(bool pChanged)
{
	mSettingsChanged = pChanged;
	onUpdateButtonState();
}


void SettingsWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
		setDeveloperTabName();
		onUpdateApplyButtonText();
	}
	QWidget::changeEvent(pEvent);
}


void SettingsWidget::setDeveloperTabName()
{
	const int index = mUi->settingsTabWidget->indexOf(mDeveloperTab.data());
	if (index > -1)
	{
		mUi->settingsTabWidget->setTabText(index, tr("Developer options"));
	}
}


void SettingsWidget::onUpdateApplyButtonText()
{
	const QWidget* const currentWidget = mUi->settingsTabWidget->currentWidget();
	if (currentWidget == mUi->generalTab || currentWidget == mDeveloperTab.data())
	{
		mUi->applyButton->setText(tr("Apply"));
	}
	else if (currentWidget == mUi->pinTab)
	{
		mUi->applyButton->setText(mUi->pinTab->getButtonText());
	}
	else if (currentWidget == mUi->readerDeviceTab)
	{
		mUi->applyButton->setText(tr("OK"));
	}
}


void SettingsWidget::onUpdateButtonState()
{
	QPushButton* const applyButton = mUi->applyButton;
	QPushButton* const cancelButton = mUi->cancelButton;

	cancelButton->setEnabled(true);

	const QWidget* const currentWidget = mUi->settingsTabWidget->currentWidget();
	if (currentWidget == mUi->generalTab || currentWidget == mDeveloperTab.data())
	{
		applyButton->setEnabled(mSettingsChanged);
	}
	else if (currentWidget == mUi->pinTab)
	{
		const bool pinButtonEnabled = mUi->pinTab->getPinButtonEnabled();
		const bool cancelButtonEnabled = mUi->pinTab->getCancelButtonEnabled();
		mUi->applyButton->setEnabled(pinButtonEnabled);
		if (pinButtonEnabled)
		{
			applyButton->setAutoDefault(true);
			applyButton->setDefault(true);
			applyButton->setFocus();
		}
		cancelButton->setEnabled(!mWorkflowRunning || cancelButtonEnabled);
	}
	else if (currentWidget == mUi->readerDeviceTab)
	{
		applyButton->setEnabled(true);
	}
}
