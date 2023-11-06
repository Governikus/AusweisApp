/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppUpdateData.h"
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
			APPCAST, PROVIDER, READER, RELEASEINFORMATION
		};

		QTimer mTimer;
		bool mUpdateScheduled;
		bool mExplicitSuccessMessage;
		const int mOneDayInMs = 1000 * 60 * 60 * 24;

		Service();
		~Service() override = default;
		void doAppUpdate(UpdateType pType, bool pForceUpdate = false);

	private Q_SLOTS:
		void onTimedUpdateTriggered();
		void onProviderUpdateFinished();
		void onReaderUpdateFinished();
		void onAppcastFinished(bool pUpdateAvailable, const GlobalStatus& pError);

	public:
		void updateAppcast();
		[[nodiscard]] bool isUpdateScheduled() const;
		Q_INVOKABLE void runUpdateIfNeeded();
		[[nodiscard]] const AppUpdateData& getUpdateData() const;

	Q_SIGNALS:
		void fireAppcastFinished(bool pUpdateAvailable, const GlobalStatus& pError);
		void fireUpdateScheduled();
};

} // namespace governikus
