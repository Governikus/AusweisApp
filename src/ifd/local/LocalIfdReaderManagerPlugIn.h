/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "EnumHelper.h"
#include "IfdReaderManagerPlugIn.h"
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

class LocalIfdReaderManagerPlugIn
	: public IfdReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

	public:
		LocalIfdReaderManagerPlugIn();

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;

	protected:
		bool isInitialPairing(const QString& pIfdName, const QString& pId) override;
		LocalIfdClient* getIfdClient() override;
		void addDispatcher(const QSharedPointer<IfdDispatcherClient>& pDispatcher) override;

	private Q_SLOTS:
		void onLocalIfdConnectionClosed(GlobalStatus::Code pCloseCode, const QString& pId);

	private:
		bool mServiceConnected;
		[[nodiscard]] bool isAusweisApp2Installed();
		void setState(LocalIfdState pState);
		void updateState();

	private Q_SLOTS:
		void connectToReader(const QSharedPointer<IfdListEntry> pIfdDevice);
};

} // namespace governikus
