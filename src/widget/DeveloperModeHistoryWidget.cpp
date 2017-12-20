/*
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#include "AppSettings.h"
#include "DeveloperModeHistoryWidget.h"
#include "LogHandler.h"
#include "ui_DeveloperModeHistoryWidget.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)


DeveloperModeHistoryWidget::DeveloperModeHistoryWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::DeveloperModeHistoryWidget())
{
	mUi->setupUi(this);

	connect(&AppSettings::getInstance(), &AppSettings::fireSettingsChanged, this, &DeveloperModeHistoryWidget::onSettingsChanged);
	connect(&LogHandler::getInstance(), &LogHandler::fireRawLog, this, &DeveloperModeHistoryWidget::onRawLog);
	connect(mUi->btnDisableDeveloperMode, &QPushButton::clicked, this, &DeveloperModeHistoryWidget::onDisableDeveloperMode);

	// initialize visibility state
	onSettingsChanged();
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


void DeveloperModeHistoryWidget::onSettingsChanged()
{
	QWidget::setVisible(AppSettings::getInstance().getGeneralSettings().isDeveloperMode());
}


void DeveloperModeHistoryWidget::onDisableDeveloperMode()
{
	AppSettings::getInstance().getGeneralSettings().setDeveloperMode(false);
	AppSettings::getInstance().getGeneralSettings().save();
}


void DeveloperModeHistoryWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
