/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "pace/KeyDerivationFunction.h"

#include <QLoggingCategory>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


KeyDerivationFunction::KeyDerivationFunction(const SecurityProtocol& pSecurityProtocol)
	: mHashAlgorithm(pSecurityProtocol.getHashAlgorithm())
	, mKeySize(pSecurityProtocol.getKeySize())
{
	if (!isInitialized())
	{
		qCCritical(card) << "Unknown algorithm:" << pSecurityProtocol;
	}
}


bool KeyDerivationFunction::isInitialized() const
{
	return mKeySize != 0;
}


QByteArray KeyDerivationFunction::enc(const QByteArray& pSecret, const QByteArray& pNonce) const
{
	return deriveKey(pSecret, pNonce, 1);
}


QByteArray KeyDerivationFunction::mac(const QByteArray& pSecret, const QByteArray& pNonce) const
{
	return deriveKey(pSecret, pNonce, 2);
}


QByteArray KeyDerivationFunction::pi(const QByteArray& pSecret) const
{
	return deriveKey(pSecret, QByteArray(), 3);
}


QByteArray KeyDerivationFunction::deriveKey(const QByteArray& pK, const QByteArray& pNonce, quint32 pC) const
{
	if (!isInitialized())
	{
		qCCritical(card) << "KeyDerivationFunction not successfully initialized";
		return QByteArray();
	}

	QByteArray counterBigEndian(4, '\0');
	qToBigEndian(pC, counterBigEndian.data());

	return QCryptographicHash::hash(pK + pNonce + counterBigEndian, mHashAlgorithm).left(mKeySize);
}
