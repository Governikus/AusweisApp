/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "IfdReaderManagerPlugin.h"
#include "LocalIfdClient.h"

#include <QTimer>

namespace governikus
{

defineEnumType(LocalIfdState
		, CONNECTED
		, DISCONNECTED
		, INVALID_CERTIFICATE
		, INCOMPATIBLE_VERSION
		, NOT_INSTALLED
		)

class LocalIfdReaderManagerPlugin
	: public IfdReaderManagerPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugin)

	public:
		LocalIfdReaderManagerPlugin();

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;

	protected:
		LocalIfdClient* getIfdClient() const override;
		void addDispatcher(const QSharedPointer<IfdDispatcherClient>& pDispatcher) override;

	private Q_SLOTS:
		void onLocalIfdConnectionClosed(GlobalStatus::Code pCloseCode, const QByteArray& pId);

	private:
		bool mServiceConnected;
#ifdef Q_OS_ANDROID
		[[nodiscard]] bool isAusweisAppInstalled();
#else
		[[nodiscard]] bool isAusweisAppInstalled() const;
#endif
		void setState(LocalIfdState pState);
		void updateState();

	private Q_SLOTS:
		void connectToReader(const QSharedPointer<IfdListEntry> pIfdDevice) const;
};

} // namespace governikus
