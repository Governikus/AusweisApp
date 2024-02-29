/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */


#include "SecurityProtocol.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


const QMap<char, ProtocolType> SecurityProtocol::cProtocol = {
	{2, ProtocolType::TA},
	{3, ProtocolType::CA},
	{4, ProtocolType::PACE},
	{5, ProtocolType::RI}
};


const QMap<char, KeyAgreementType> SecurityProtocol::cKeyAgreement = {
	{1, KeyAgreementType::DH},
	{2, KeyAgreementType::ECDH},
	{3, KeyAgreementType::DH},
	{4, KeyAgreementType::ECDH},
	{6, KeyAgreementType::ECDH}
};


const QMap<char, MappingType> SecurityProtocol::cMapping = {
	{1, MappingType::GM},
	{2, MappingType::GM},
	{3, MappingType::IM},
	{4, MappingType::IM},
	{6, MappingType::CAM}
};


const QMap<char, CipherType> SecurityProtocol::cCipher = {
	{1, CipherType::DES3_CBC},
	{2, CipherType::AES_128_CBC},
	{3, CipherType::AES_192_CBC},
	{4, CipherType::AES_256_CBC}
};

const QMap<char, SignatureType> SecurityProtocol::cSignature = {
	{1, SignatureType::RSA},
	{2, SignatureType::ECDSA}
};


const QMap<char, std::optional<QCryptographicHash::Algorithm>> SecurityProtocol::cHashAlgorithm = {
	{1, std::optional<QCryptographicHash::Algorithm>(QCryptographicHash::Sha1)},
	{2, std::optional<QCryptographicHash::Algorithm>(QCryptographicHash::Sha224)},
	{3, std::optional<QCryptographicHash::Algorithm>(QCryptographicHash::Sha256)},
	{4, std::optional<QCryptographicHash::Algorithm>(QCryptographicHash::Sha384)},
	{5, std::optional<QCryptographicHash::Algorithm>(QCryptographicHash::Sha512)}
};


const QMap<char, std::optional<QCryptographicHash::Algorithm>> SecurityProtocol::cRsaHashAlgorithm = {
	{1, std::optional<QCryptographicHash::Algorithm>(QCryptographicHash::Sha1)},
	{2, std::optional<QCryptographicHash::Algorithm>(QCryptographicHash::Sha256)},
	{3, std::optional<QCryptographicHash::Algorithm>(QCryptographicHash::Sha1)},
	{4, std::optional<QCryptographicHash::Algorithm>(QCryptographicHash::Sha256)},
	{5, std::optional<QCryptographicHash::Algorithm>(QCryptographicHash::Sha512)},
	{6, std::optional<QCryptographicHash::Algorithm>(QCryptographicHash::Sha512)}
};


SecurityProtocol::SecurityProtocol(const Oid& pOid)
	: mOid(pOid)
	, mProtocol(ProtocolType::UNDEFINED)
	, mKeyAgreement(KeyAgreementType::UNDEFINED)
	, mMapping(MappingType::UNDEFINED)
	, mCipher(CipherType::UNDEFINED)
	, mSignature(SignatureType::UNDEFINED)
	, mHashAlgorithm(std::nullopt)
{
	const QByteArray rawOid = mOid.getData();
	if (rawOid.size() < 8 || !rawOid.startsWith(Oid(KnownOid::BSI_DE_PROTOCOLS_SMARTCARD).getData()))
	{
		qCCritical(card) << "Unknown OID:" << mOid;
		return;
	}

	mProtocol = cProtocol.value(rawOid.at(7), ProtocolType::UNDEFINED);
	if (mProtocol == ProtocolType::UNDEFINED)
	{
		qCCritical(card) << "Unsupported OID:" << mOid;
		return;
	}

	if (rawOid.size() < 10)
	{
		return;
	}

	const char keyAgreement = rawOid.at(8);
	const char algorithm = rawOid.at(9);

	const auto* hashBase = &cHashAlgorithm;
	switch (mProtocol)
	{
		case ProtocolType::TA:
			mSignature = cSignature.value(keyAgreement, SignatureType::UNDEFINED);
			if (mSignature == SignatureType::UNDEFINED)
			{
				qCCritical(card) << "Unsupported OID:" << mOid;
				return;
			}
			if (mSignature == SignatureType::RSA)
			{
				hashBase = &cRsaHashAlgorithm;
			}
			Q_FALLTHROUGH();

		case ProtocolType::RI:
			mHashAlgorithm = hashBase->value(algorithm, std::nullopt);
			if (!mHashAlgorithm.has_value())
			{
				qCCritical(card) << "Unknown OID:" << mOid;
			}
			return;

		case ProtocolType::PACE:
			mMapping = cMapping.value(keyAgreement, MappingType::UNDEFINED);
			if (mMapping == MappingType::UNDEFINED)
			{
				qCCritical(card) << "Unknown OID:" << mOid;
				return;
			}
			Q_FALLTHROUGH();

		case ProtocolType::CA:
			mKeyAgreement = cKeyAgreement.value(keyAgreement, KeyAgreementType::UNDEFINED);
			if (mKeyAgreement == KeyAgreementType::UNDEFINED)
			{
				qCCritical(card) << "Unknown OID:" << mOid;
				return;
			}
			mCipher = cCipher.value(algorithm, CipherType::UNDEFINED);
			if (mCipher == CipherType::UNDEFINED)
			{
				qCCritical(card) << "Unknown OID:" << mOid;
				return;
			}
			break;

		default:
			qCCritical(card) << "Unknown OID:" << mOid;

	}
}


const Oid& SecurityProtocol::getOid() const
{
	return mOid;
}


ProtocolType SecurityProtocol::getProtocol() const
{
	return mProtocol;
}


KeyAgreementType SecurityProtocol::getKeyAgreement() const
{
	return mKeyAgreement;
}


MappingType SecurityProtocol::getMapping() const
{
	return mMapping;
}


const EVP_CIPHER* SecurityProtocol::getCipher() const
{
	switch (mCipher)
	{
		case CipherType::AES_128_CBC:
			return EVP_aes_128_cbc();

		case CipherType::AES_192_CBC:
			return EVP_aes_192_cbc();

		case CipherType::AES_256_CBC:
			return EVP_aes_256_cbc();

		case CipherType::DES3_CBC:
			qCCritical(card) << "3DES not supported";
			return nullptr;

		default:
			return nullptr;
	}
}


const char* SecurityProtocol::getCipherString() const
{
	switch (mCipher)
	{
		case CipherType::AES_128_CBC:
			return "aes-128-cbc";

		case CipherType::AES_192_CBC:
			return "aes-192-cbc";

		case CipherType::AES_256_CBC:
			return "aes-256-cbc";

		case CipherType::DES3_CBC:
			qCCritical(card) << "3DES not supported";
			return nullptr;

		default:
			return nullptr;
	}
}


int SecurityProtocol::getKeySize() const
{
	switch (mCipher)
	{
		case CipherType::AES_128_CBC:
			return 16;

		case CipherType::AES_192_CBC:
			return 24;

		case CipherType::AES_256_CBC:
			return 32;

		case CipherType::DES3_CBC:
			qCCritical(card) << "3DES not supported";
			return 0; // mark as invalid

		default:
			return 0;
	}
}


SignatureType SecurityProtocol::getSignature() const
{
	return mSignature;
}


QCryptographicHash::Algorithm SecurityProtocol::getHashAlgorithm() const
{
	switch (mCipher)
	{
		case CipherType::AES_128_CBC:
			return QCryptographicHash::Sha1;

		case CipherType::AES_192_CBC:
		case CipherType::AES_256_CBC:
			return QCryptographicHash::Sha256;

		case CipherType::DES3_CBC:
			qCCritical(card) << "3DES not supported";
			return QCryptographicHash::Sha256;

		default:
			if (mHashAlgorithm.has_value())
			{
				return mHashAlgorithm.value();
			}

			qCCritical(card) << "Unknown hash algorithm:" << mOid;
			return QCryptographicHash::Sha256;
	}
}


bool SecurityProtocol::operator==(const SecurityProtocol& pOther) const
{
	return mOid == pOther.mOid;
}


#include "moc_SecurityProtocol.cpp"
