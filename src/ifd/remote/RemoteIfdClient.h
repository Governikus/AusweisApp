/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DatagramHandler.h"
#include "Env.h"
#include "IfdClientImpl.h"


class MockRemoteIfdClient;
class test_RemoteIfdClient;


#ifdef Q_OS_ANDROID
extern "C" {
JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_AusweisApp2LocalIfdService_notifyLocalIfdDisabled(JNIEnv*, jobject);
}
#endif


namespace governikus
{

class RemoteIfdClient
	: public IfdClientImpl
{
	Q_OBJECT
	friend class Env;
#ifdef Q_OS_ANDROID
	friend void ::Java_com_governikus_ausweisapp2_AusweisApp2LocalIfdService_notifyLocalIfdDisabled(JNIEnv*, jobject);
#endif
	friend class ::MockRemoteIfdClient;
	friend class ::test_RemoteIfdClient;

	private:
		QSharedPointer<DatagramHandler> mDatagramHandler;
		QScopedPointer<IfdList> mIfdList;

		RemoteIfdClient();
		void resetDatagramHandler();
		[[nodiscard]] bool enableLocalIfdDetection(bool pEnabled) const;

	private Q_SLOTS:
		void onNewMessage(const QByteArray& pData, const QHostAddress& pAddress);

	public:
		Q_INVOKABLE void startDetection() override;
		Q_INVOKABLE void stopDetection() override;
		Q_INVOKABLE bool isDetecting() override;

		[[nodiscard]] QList<QSharedPointer<IfdListEntry>> getAnnouncingRemoteDevices() const override;
		Q_INVOKABLE void requestRemoteDevices()  override;
		QList<RemoteServiceSettings::RemoteInfo> getConnectedDeviceInfos() override;
};


} // namespace governikus
