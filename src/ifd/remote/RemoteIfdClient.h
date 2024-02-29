/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Send RemoteReaderDiscoverCmds and maintain a list of responsive peers.
 */

#pragma once

#include "DatagramHandler.h"
#include "Env.h"
#include "IfdClientImpl.h"

class test_RemoteIfdClient;

namespace governikus
{

class RemoteIfdClient
	: public IfdClientImpl
{
	Q_OBJECT
	friend class Env;
	friend class ::test_RemoteIfdClient;

	private:
		QSharedPointer<DatagramHandler> mDatagramHandler;
		QScopedPointer<IfdList> mIfdList;

		RemoteIfdClient();

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
