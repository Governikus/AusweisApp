/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "asn1/KnownOIDs.h"
#include "pace/KeyDerivationFunction.h"

#include <QLoggingCategory>
#include <openssl/sha.h>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


KeyDerivationFunction::KeyDerivationFunction(const QByteArray& pPaceAlgorithm)
	: mHashAlgorithm()
	, mKeySize(0)
{
	using namespace KnownOIDs;

	if (pPaceAlgorithm == id_PACE::DH::GM_3DES_CBC_CBC || pPaceAlgorithm == id_PACE::DH::IM_3DES_CBC_CBC || pPaceAlgorithm == id_PACE::ECDH::GM_3DES_CBC_CBC
			|| pPaceAlgorithm == id_PACE::ECDH::IM_3DES_CBC_CBC)
	{
		mHashAlgorithm = QCryptographicHash::Sha1;
		mKeySize = 8;
	}
	else if (pPaceAlgorithm == id_PACE::DH::GM_AES_CBC_CMAC_128 || pPaceAlgorithm == id_PACE::DH::IM_AES_CBC_CMAC_128 || pPaceAlgorithm == id_PACE::ECDH::GM_AES_CBC_CMAC_128
			|| pPaceAlgorithm == id_PACE::ECDH::IM_AES_CBC_CMAC_128)
	{
		mHashAlgorithm = QCryptographicHash::Sha1;
		mKeySize = 16;
	}
	else if (pPaceAlgorithm == id_PACE::DH::GM_AES_CBC_CMAC_192 || pPaceAlgorithm == id_PACE::DH::IM_AES_CBC_CMAC_192 || pPaceAlgorithm == id_PACE::ECDH::GM_AES_CBC_CMAC_192
			|| pPaceAlgorithm == id_PACE::ECDH::IM_AES_CBC_CMAC_192)
	{
		mHashAlgorithm = QCryptographicHash::Sha256;
		mKeySize = 24;
	}
	else if (pPaceAlgorithm == id_PACE::DH::GM_AES_CBC_CMAC_256 || pPaceAlgorithm == id_PACE::DH::IM_AES_CBC_CMAC_256 || pPaceAlgorithm == id_PACE::ECDH::GM_AES_CBC_CMAC_256
			|| pPaceAlgorithm == id_PACE::ECDH::IM_AES_CBC_CMAC_256)
	{
		mHashAlgorithm = QCryptographicHash::Sha256;
		mKeySize = 32;
	}
	else
	{
		qCCritical(card) << "Unknown algorithm: " << pPaceAlgorithm;
	}
}


KeyDerivationFunction::~KeyDerivationFunction()
{
}


bool KeyDerivationFunction::isInitialized()
{
	return mKeySize != 0;
}


QByteArray KeyDerivationFunction::enc(const QByteArray& pSecret)
{
	return deriveKey(pSecret, QByteArray(), 1);
}


QByteArray KeyDerivationFunction::mac(const QByteArray& pSecret)
{
	return deriveKey(pSecret, QByteArray(), 2);
}


QByteArray KeyDerivationFunction::pi(const QString& pSecret)
{
	return deriveKey(pSecret.toLatin1(), QByteArray(), 3);
}


QByteArray KeyDerivationFunction::deriveKey(const QByteArray& pK, const QByteArray& pNonce, uint32_t pC)
{
	if (!isInitialized())
	{
		qCCritical(card) << "KeyDerivationFunction not successfully initialized";
		return QByteArray();
	}

	QByteArray dataBytes(pK);
	dataBytes.append(pNonce);
	dataBytes.append((pC >> 24) & 0xFF);
	dataBytes.append((pC >> 16) & 0xFF);
	dataBytes.append((pC >> 8) & 0xFF);
	dataBytes.append(pC & 0xFF);

	QByteArray hashBytes;
	if (mHashAlgorithm == QCryptographicHash::Sha1)
	{
		char md[SHA_DIGEST_LENGTH];
		SHA1(reinterpret_cast<const uchar*>(dataBytes.constData()), dataBytes.size(), reinterpret_cast<uchar*>(md));
		hashBytes.append(md, mKeySize);
	}
	else if (mHashAlgorithm == QCryptographicHash::Sha256)
	{
		char md[SHA256_DIGEST_LENGTH];
		SHA256(reinterpret_cast<const uchar*>(dataBytes.constData()), dataBytes.size(), reinterpret_cast<uchar*>(md));
		hashBytes.append(md, mKeySize);
	}
	return hashBytes;
}
