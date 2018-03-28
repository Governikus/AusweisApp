/*!
 * \brief Information needed to display a remote device in the GUI and to open
 * a websocket connection to it.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "messages/IfdVersion.h"

#include <QHostAddress>
#include <QSharedDataPointer>
#include <QUrl>
#include <QVector>


namespace governikus
{
class Discovery;


class RemoteDeviceDescriptor
{
	private:
		class RemoteDeviceDescriptorData
			: public QSharedData
		{
			public:
				RemoteDeviceDescriptorData(const QString& pIfdName,
						const QString& pIfdId,
						const QVector<IfdVersion::Version>& pApiVersions,
						const QUrl& pUrl);

				virtual ~RemoteDeviceDescriptorData();

				const QString mIfdName;
				const QString mIfdId;
				const QVector<IfdVersion::Version> mApiVersions;
				const QUrl mUrl;
				bool operator==(const RemoteDeviceDescriptorData& pOther) const;
		};

		static RemoteDeviceDescriptorData* createRemoteDeviceDescriptorData(const QSharedPointer<const Discovery>& pMsg,
				const QHostAddress& pHostAddress,
				QAbstractSocket::NetworkLayerProtocol pRequiredProtocol);

		const QSharedDataPointer<RemoteDeviceDescriptorData> d;

	public:
		RemoteDeviceDescriptor() = default;

		RemoteDeviceDescriptor(const QSharedPointer<const Discovery>& pDiscovery,
				const QHostAddress& pHostAddress,
				QAbstractSocket::NetworkLayerProtocol pRequiredProtocol = QAbstractSocket::IPv4Protocol);

		~RemoteDeviceDescriptor() = default;

		const QString& getIfdName() const;
		const QString& getIfdId() const;
		const QVector<IfdVersion::Version>& getApiVersions() const;
		bool isSupported() const;
		const QUrl& getUrl() const;
		bool isNull() const;

		bool operator==(const RemoteDeviceDescriptor& pOther) const;

};


inline QDebug operator<<(QDebug pDbg, const RemoteDeviceDescriptor& pRemoteDeviceDescriptor)
{
	QDebugStateSaver saver(pDbg);
	return pDbg.noquote().nospace() << "RemoteDevice(" << pRemoteDeviceDescriptor.getIfdName() << ", " <<
		   pRemoteDeviceDescriptor.getIfdId() << ", " <<
		   pRemoteDeviceDescriptor.getUrl() << ", " <<
		   pRemoteDeviceDescriptor.getApiVersions() << ")";
}


} /* namespace governikus */
