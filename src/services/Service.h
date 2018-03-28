/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppUpdater.h"

#include <QTimer>

namespace governikus
{
class Service
	: public QObject
{
	Q_OBJECT

	private:
		QTimer mTimer;
		bool mUpdateScheduled;
		bool mExplicitSuccessMessage;
		AppUpdater mAppUpdater;
		const int mOneDayInMs = 1000 * 60 * 60 * 24;

	protected:
		Service();
		~Service() = default;

	private Q_SLOTS:
		void doConfigurationsUpdate();
		void doAppUpdate(bool pIgnoreNextVersionskip);
		void onTimedUpdateTriggered();
		void onAppUpdateFinished(bool pUpdateAvailable, const GlobalStatus& pError);

	public:
		static Service& getInstance();
		void updateConfigurations();
		void updateApp(bool pIgnoreNextVersionskip = false);
		bool isUpdateScheduled();
		void runUpdateIfNeeded();
		const AppUpdateData& getUpdateData() const;

	Q_SIGNALS:
		void fireAppUpdateFinished(bool pUpdateAvailable, const GlobalStatus& pError);
		void fireUpdateScheduled();
};

} /* namespace governikus */
