/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Information needed to display a remote device in the GUI and to open
 * a websocket connection to it.
 */

#pragma once

#include "messages/Discovery.h"

#include <QHostAddress>
#include <QList>
#include <QSharedDataPointer>
#include <QUrl>


namespace governikus
{

class IfdDescriptor
{
	private:
		class IfdDescriptorData
			: public QSharedData
		{
			public:
				IfdDescriptorData(const QString& pIfdName,
						const QString& pIfdId,
						const QList<IfdVersion::Version>& pApiVersions,
						const bool pIsPairingAnnounced,
						const QUrl& pUrl,
						bool pIsLocalIfd);

				virtual ~IfdDescriptorData();

				const QString mIfdName;
				const QString mIfdId;
				const QList<IfdVersion::Version> mApiVersions;
				const bool mIsPairingAnnounced;
				const QUrl mUrl;
				const bool mIsLocalIfd;

				bool operator==(const IfdDescriptorData& pOther) const;
				bool isSameIfd(const IfdDescriptorData& pOther) const;
		};

		QSharedDataPointer<IfdDescriptorData> d;

	public:
		IfdDescriptor() = default;
		IfdDescriptor(const Discovery& pDiscovery, const QHostAddress& pHostAddress, bool pLocalIfd = false);

		~IfdDescriptor() = default;

		[[nodiscard]] const QString& getIfdName() const;
		[[nodiscard]] const QString& getIfdId() const;
		[[nodiscard]] const QList<IfdVersion::Version>& getApiVersions() const;
		[[nodiscard]] bool isSupported() const;
		[[nodiscard]] bool isPairingAnnounced() const;
		[[nodiscard]] const QUrl& getUrl() const;
		[[nodiscard]] bool isNull() const;
		[[nodiscard]] bool isLocalIfd() const;

		bool operator==(const IfdDescriptor& pOther) const;
		[[nodiscard]] bool isSameIfd(const IfdDescriptor& pOther) const;

};


inline QDebug operator<<(QDebug pDbg, const IfdDescriptor& pIfdDescriptor)
{
	QDebugStateSaver saver(pDbg);
	return pDbg.noquote().nospace() << "IFD(" << pIfdDescriptor.getIfdName() << ", " <<
		   pIfdDescriptor.getIfdId() << ", " <<
		   pIfdDescriptor.getUrl() << ", " <<
		   pIfdDescriptor.getApiVersions() << ")";
}


} // namespace governikus
