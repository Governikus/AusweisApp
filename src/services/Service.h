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
		enum class UpdateType
		{
			APPCAST, PROVIDER, READER
		};

		QTimer mTimer;
		bool mUpdateScheduled;
		bool mExplicitSuccessMessage;
		const int mOneDayInMs = 1000 * 60 * 60 * 24;

		Service();
		virtual ~Service() = default;
		void doAppUpdate(UpdateType pType, bool pForceUpdate = false);

	private Q_SLOTS:
		void onTimedUpdateTriggered();
		void onProviderUpdateFinished();
		void onAppcastFinished(bool pUpdateAvailable, const GlobalStatus& pError);

	public:
		void updateAppcast();
		bool isUpdateScheduled() const;
		Q_INVOKABLE void runUpdateIfNeeded();
		const AppUpdateData& getUpdateData() const;

	Q_SIGNALS:
		void fireAppcastFinished(bool pUpdateAvailable, const GlobalStatus& pError);
		void fireUpdateScheduled();
};

} // namespace governikus
