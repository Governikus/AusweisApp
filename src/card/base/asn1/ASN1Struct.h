/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "SmartCardDefinitions.h"
#include "asn1/Oid.h"

#include <QByteArray>
#include <QList>
#include <openssl/asn1.h>


namespace governikus
{

class ASN1Struct
{
	public:
		enum DATA_TAG : int
		{
			NONE = -1,

			// According to TR-03110-3, chapter B.(1|2|3), B.14.* and C3.2
			CERTIFICATE_EXTENSION_CONTENT_0 = 0,
			CRYPTOGRAPHIC_MECHANISM_REFERENCE = 0,
			CA_EPHEMERAL_PUBLIC_KEY = 0,
			MAPPING_DATA = 1,
			RI_FIRST_IDENTIFIER = 1,
			PACE_EPHEMERAL_PUBLIC_KEY = 3,
			PUBLIC_KEY_REFERENCE = 3,
			PRIVATE_KEY_REFERENCE = 4,
			AUTHENTICATION_TOKEN = 5,
			EC_PUBLIC_POINT = 6,
			AUXILIARY_AUTHENTICATED_DATA = 7,
			TA_EPHEMERAL_PUBLIC_KEY = 17,
			DYNAMIC_AUTHENTICATION_DATA = 28,
			CERTIFICATE_SIGNATURE = 55,
			PUBLIC_KEY = 73,
			CERTIFICATE_HOLDER_AUTHORIZATION_TEMPLATE = 76,
			CERTIFICATE_BODY = 78,

			// According to ASN.1
			UNI_BOOLEAN = 1,
			UNI_INTEGER = 2,
			UNI_BITSTRING = 3,
			UNI_OCTETSTRING = 4,
			UNI_NULL = 5,
			UNI_OBJECT_IDENTIFIER = 6,
			UNI_SEQUENCE = 16,
			UNI_SET = 17
		};

	private:
		int mClass;
		int mTag;
		QByteArray mSimpleData;
		QList<ASN1Struct> mComplexData;

	public:
		explicit ASN1Struct(int pClass = V_ASN1_UNIVERSAL, DATA_TAG pTag = NONE, const QByteArray& pData = QByteArray());
		explicit ASN1Struct(const QByteArray& pData);

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
		void print(QDebug pDbg, int pLevel) const;
#endif
};

QDebug operator<<(QDebug pDbg, const ASN1Struct& pASN1Struct);

} // namespace governikus
