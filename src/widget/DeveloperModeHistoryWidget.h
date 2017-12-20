/*!
 * \brief A Widget to display developer mode errors which occurred
 *
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
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

	private:
		QScopedPointer<Ui::DeveloperModeHistoryWidget> mUi;

		void appendLoggingDump(const QString& pLog);

	private Q_SLOTS:
		void onDisableDeveloperMode();

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		explicit DeveloperModeHistoryWidget(QWidget* pParent = nullptr);
		virtual ~DeveloperModeHistoryWidget() override;

	public Q_SLOTS:
		void onRawLog(const QString& pMsg, const QString& pCategoryName);
		void onSettingsChanged();
};

} /* namespace governikus */
