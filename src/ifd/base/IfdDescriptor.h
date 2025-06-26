/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
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
				IfdDescriptorData(const Discovery& pDiscovery,
						bool pIsLocalIfd);

				virtual ~IfdDescriptorData();

				const Discovery mDiscovery;
				const bool mIsLocalIfd;

				bool isSameIfd(const IfdDescriptorData& pOther) const;
		};

		QSharedDataPointer<IfdDescriptorData> d;

	public:
		IfdDescriptor() = default;
		explicit IfdDescriptor(const Discovery& pDiscovery, bool pLocalIfd = false);
		~IfdDescriptor() = default;

		[[nodiscard]] const QString& getIfdName() const;
		[[nodiscard]] const QByteArray& getIfdId() const;
		[[nodiscard]] const QList<IfdVersion::Version>& getSupportedApis() const;
		[[nodiscard]] bool isSupported() const;
		[[nodiscard]] bool isPairing() const;
		[[nodiscard]] const QList<QUrl>& getAddresses() const;
		[[nodiscard]] bool isNull() const;
		[[nodiscard]] bool isLocalIfd() const;

		[[nodiscard]] bool isSameIfd(const IfdDescriptor& pOther) const;

};


inline QDebug operator<<(QDebug pDbg, const IfdDescriptor& pIfdDescriptor)
{
	QDebugStateSaver saver(pDbg);
	return pDbg.noquote().nospace() << "IFD(" << pIfdDescriptor.getIfdName() << ", " <<
		   pIfdDescriptor.getIfdId() << ", " <<
		   pIfdDescriptor.getAddresses() << ", " <<
		   pIfdDescriptor.getSupportedApis() << ")";
}


} // namespace governikus
