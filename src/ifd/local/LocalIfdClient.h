/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdClientImpl.h"
#include "Randomizer.h"

#include <functional>

#ifdef Q_OS_ANDROID
	#include <QJniObject>
#endif

namespace governikus
{

class LocalIfdClient
	: public IfdClientImpl
{
	Q_OBJECT
	friend LocalIfdClient* createNewObject<LocalIfdClient*>();

	private:
		QString mPsk;
		QSharedPointer<IfdListEntry> mDevice;
#ifdef Q_OS_ANDROID
		QJniObject mServiceConnection;
#endif

	public:
		LocalIfdClient();
		~LocalIfdClient() override;

		void startDetection() override;
		void stopDetection() override;
		bool isDetecting() override;
		QVector<RemoteServiceSettings::RemoteInfo> getConnectedDeviceInfos() override;

		[[nodiscard]] const QString& getPsk() const;

		void serviceConnected();
		void serviceDisconnected();
};
} // namespace governikus
