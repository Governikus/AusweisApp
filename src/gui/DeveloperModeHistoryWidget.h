/*!
 * DeveloperModeHistoryWidget.h
 *
 * \brief A Widget to display developer mode errors which occurred
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */
#pragma once

#include <QWidget>


namespace Ui
{
class DeveloperModeHistoryWidget;
}


namespace governikus
{

class DeveloperModeHistoryWidget
	: public QWidget
{
	Q_OBJECT

	public:
		explicit DeveloperModeHistoryWidget(QWidget* pParent = nullptr);
		virtual ~DeveloperModeHistoryWidget();

	public Q_SLOTS:
		void onRawLog(const QString& pMsg, const QString& pCategoryName);
		void onSettingsChanged();

	private:
		QScopedPointer<Ui::DeveloperModeHistoryWidget> mUi;

		void appendLoggingDump(const QString& pLog);

	private Q_SLOTS:
		void onDisableDeveloperMode();
};

} /* namespace governikus */
