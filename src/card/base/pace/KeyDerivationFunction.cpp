/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/KnownOIDs.h"
#include "pace/KeyDerivationFunction.h"

#include <QLoggingCategory>
#include <QtEndian>

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
		qCCritical(card) << "3DES not supported";
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


QByteArray KeyDerivationFunction::deriveKey(const QByteArray& pK, const QByteArray& pNonce, quint32 pC)
{
	if (!isInitialized())
	{
		qCCritical(card) << "KeyDerivationFunction not successfully initialized";
		return QByteArray();
	}

	char counterBigEndian[4];
	qToBigEndian(pC, counterBigEndian);

	return QCryptographicHash::hash(pK + pNonce + QByteArray(counterBigEndian, 4), mHashAlgorithm).left(mKeySize);
}
