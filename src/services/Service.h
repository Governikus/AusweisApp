/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppUpdater.h"
#include "Env.h"

#include <QTimer>

namespace governikus
{
class Service
	: public QObject
{
	Q_OBJECT
	friend class Env;

	private:
		QTimer mTimer;
		bool mUpdateScheduled;
		bool mExplicitSuccessMessage;
		const int mOneDayInMs = 1000 * 60 * 60 * 24;

	protected:
		Service();
		virtual ~Service() = default;
		static Service& getInstance();

	private Q_SLOTS:
		void doConfigurationsUpdate();
		void doAppUpdate(bool pIgnoreNextVersionskip);
		void onTimedUpdateTriggered();
		void onAppUpdateFinished(bool pUpdateAvailable, const GlobalStatus& pError);

	public:
		void updateConfigurations();
		void updateApp(bool pIgnoreNextVersionskip = false);
		bool isUpdateScheduled();
		Q_INVOKABLE void runUpdateIfNeeded();
		const AppUpdateData& getUpdateData() const;

	Q_SIGNALS:
		void fireAppUpdateFinished(bool pUpdateAvailable, const GlobalStatus& pError);
		void fireUpdateScheduled();
};

} // namespace governikus
