/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "DeveloperModeHistoryWidget.h"
#include "ui_DeveloperModeHistoryWidget.h"

#include "AppSettings.h"
#include "LogHandler.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)


DeveloperModeHistoryWidget::DeveloperModeHistoryWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::DeveloperModeHistoryWidget())
{
	mUi->setupUi(this);


	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(&generalSettings, &GeneralSettings::fireDeveloperOptionsChanged, this, &DeveloperModeHistoryWidget::onDeveloperOptionsChanged);
	connect(Env::getSingleton<LogHandler>(), &LogHandler::fireRawLog, this, &DeveloperModeHistoryWidget::onRawLog);
	connect(mUi->btnDisableDeveloperMode, &QPushButton::clicked, this, &DeveloperModeHistoryWidget::onDisableDeveloperMode);

	// initialize visibility state
	onDeveloperOptionsChanged();
}


DeveloperModeHistoryWidget::~DeveloperModeHistoryWidget()
{
}


void DeveloperModeHistoryWidget::appendLoggingDump(const QString& pLog)
{
	QString formatted = pLog;

	// Remove outer quotation
	const QLatin1Char quote('"');
	if (formatted.startsWith(quote) && formatted.endsWith(quote))
	{
		formatted = formatted.mid(1, formatted.length() - 2);
	}

	mUi->plainTextEdit->appendHtml(QStringLiteral("<pre><font color='red'>%1</font></pre>").arg(formatted));
}


void DeveloperModeHistoryWidget::onRawLog(const QString& pMsg, const QString& pCategoryName)
{
	static const QString categoryDevMode = QString::fromLatin1(developermode().categoryName());
	if (pCategoryName == categoryDevMode)
	{
		appendLoggingDump(pMsg);
	}
}


void DeveloperModeHistoryWidget::onDeveloperOptionsChanged()
{
	QWidget::setVisible(Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode());
}


void DeveloperModeHistoryWidget::onDisableDeveloperMode()
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setDeveloperMode(false);
	Env::getSingleton<AppSettings>()->getGeneralSettings().save();
}


void DeveloperModeHistoryWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
