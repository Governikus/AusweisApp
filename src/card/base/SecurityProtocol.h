/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "asn1/Oid.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QMap>
#include <openssl/evp.h>

#include <optional>


namespace governikus
{

defineEnumType(ProtocolType, UNDEFINED, TA, CA, PACE, RI)
defineEnumType(KeyAgreementType, UNDEFINED, DH, ECDH)
defineEnumType(MappingType, UNDEFINED, GM, IM, CAM)
defineEnumType(CipherType, UNDEFINED, DES3_CBC, AES_128_CBC, AES_192_CBC, AES_256_CBC)
defineEnumType(SignatureType, UNDEFINED, RSA, ECDSA)

class SecurityProtocol
{
	private:
		static const QMap<char, ProtocolType> cProtocol;
		static const QMap<char, KeyAgreementType> cKeyAgreement;
		static const QMap<char, MappingType> cMapping;
		static const QMap<char, CipherType> cCipher;
		static const QMap<char, SignatureType> cSignature;
		static const QMap<char, std::optional<QCryptographicHash::Algorithm>> cHashAlgorithm;
		static const QMap<char, std::optional<QCryptographicHash::Algorithm>> cRsaHashAlgorithm;

		const Oid mOid;

		ProtocolType mProtocol;
		KeyAgreementType mKeyAgreement;
		MappingType mMapping;
		CipherType mCipher;
		SignatureType mSignature;
		std::optional<QCryptographicHash::Algorithm> mHashAlgorithm;

		void logCritical(const QLatin1String& pTopic) const;

	public:
		explicit SecurityProtocol(const Oid& pOid);

		[[nodiscard]] const Oid& getOid() const;
		[[nodiscard]] ProtocolType getProtocol() const;
		[[nodiscard]] KeyAgreementType getKeyAgreement() const;
		[[nodiscard]] MappingType getMapping() const;
		[[nodiscard]] const EVP_CIPHER* getCipher() const;
		[[nodiscard]] const char* getCipherString() const;
		[[nodiscard]] int getKeySize() const;
		[[nodiscard]] SignatureType getSignature() const;
		[[nodiscard]] QCryptographicHash::Algorithm getHashAlgorithm() const;

		bool operator==(const SecurityProtocol& pOther) const;
};


inline QDebug operator<<(QDebug pDbg, const SecurityProtocol& pProtocol)
{
	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << QByteArray(pProtocol.getOid());
	return pDbg.space();
}


}  // namespace governikus
