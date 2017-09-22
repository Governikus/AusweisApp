/*!
 * HashAlgorithmUtil.h
 *
 * \brief Simple utility class for QCryptographicHash::Algorithm.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QCryptographicHash>
#include <QDebug>
#include <QString>

class test_HashAlgorithmUtil;

namespace governikus
{

#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)

class HashAlgorithmUtil
{
	HashAlgorithmUtil() = delete;
	~HashAlgorithmUtil() = delete;

	friend QDebug operator<<(QDebug, QCryptographicHash::Algorithm);
	friend class ::test_HashAlgorithmUtil;

	/*!
	 * Determines a readable name for the hash algorithm.
	 */
	static QString getName(QCryptographicHash::Algorithm pAlgorithm)
	{
		switch (pAlgorithm)
		{
	#ifndef QT_CRYPTOGRAPHICHASH_ONLY_SHA1
			case QCryptographicHash::Md4:
				return QStringLiteral("MD4");

			case QCryptographicHash::Md5:
				return QStringLiteral("MD5");

	#endif
			case QCryptographicHash::Sha1:
				return QStringLiteral("SHA1");

	#ifndef QT_CRYPTOGRAPHICHASH_ONLY_SHA1
			case QCryptographicHash::Sha224:
				return QStringLiteral("SHA224");

			case QCryptographicHash::Sha256:
				return QStringLiteral("SHA256");

			case QCryptographicHash::Sha384:
				return QStringLiteral("SHA384");

			case QCryptographicHash::Sha512:
				return QStringLiteral("SHA512");

			case QCryptographicHash::Sha3_224:
				return QStringLiteral("SHA3_224");

			case QCryptographicHash::Sha3_256:
				return QStringLiteral("SHA3_256");

			case QCryptographicHash::Sha3_384:
				return QStringLiteral("SHA3_384");

			case QCryptographicHash::Sha3_512:
				return QStringLiteral("SHA3_512");

	#endif
		}

		Q_UNREACHABLE();
	}


};


inline QDebug operator<<(QDebug pDbg, QCryptographicHash::Algorithm pAlgo)
{
	QDebugStateSaver saver(pDbg);
	pDbg.noquote() << HashAlgorithmUtil::getName(pAlgo);
	return pDbg;
}


#endif

} /* namespace governikus */
