/*!
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "SmartCardDefinitions.h"
#include "asn1/Oid.h"

#include <QByteArray>
#include <QVector>
#include <openssl/asn1.h>


namespace governikus
{

class CommandData
{
	public:
		// According to TR-03110-3, chapter B.(1|2|3) and B.14.*
		enum DATA_TAG : int
		{
			NONE = -1,
			CRYPTOGRAPHIC_MECHANISM_REFERENCE = 0,
			CA_EPHEMERAL_PUBLIC_KEY = 0,
			MAPPING_DATA = 1,
			INTEGER = 2,
			PACE_EPHEMERAL_PUBLIC_KEY = 3,
			PUBLIC_KEY_REFERENCE = 3,
			PRIVATE_KEY_REFERENCE = 4,
			AUTHENTICATION_TOKEN = 5,
			RI_EPHEMERAL_PUBLIC_KEY = 6,
			AUXILIARY_AUTHENTICATED_DATA = 7,
			TA_EPHEMERAL_PUBLIC_KEY = 17,
			DYNAMIC_AUTHENTICATION_DATA = 28,
			CERTIFICATE_SIGNATURE = 55,
			CERTIFICATE_HOLDER_AUTHORIZATION_TEMPLATE = 76,
			CERTIFICATE_BODY = 78
		};

	private:
		int mClass;
		int mTag;
		QByteArray mSimpleData;
		QVector<CommandData> mComplexData;

	public:
		explicit CommandData(int pClass = V_ASN1_UNIVERSAL, DATA_TAG pTag = NONE, const QByteArray& pData = QByteArray());
		explicit CommandData(const QByteArray& pData);

		[[nodiscard]] QByteArray getData(int pClass, DATA_TAG pTag) const;
		[[nodiscard]] QByteArray getObject(int pClass, DATA_TAG pTag) const;

		void append(const QByteArray& pData);
		void append(DATA_TAG pTag, int pValue);
		void append(DATA_TAG pTag, const Oid& pOid);
		void append(DATA_TAG pTag, PacePasswordId pPassword);
		void append(DATA_TAG pTag, const QByteArray& pData);

		operator QByteArray() const;

#ifndef QT_NO_DEBUG
		int getObjectCount() const;
#endif
};

} // namespace governikus
