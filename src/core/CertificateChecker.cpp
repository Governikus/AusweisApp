/*!
 * CertificateChecker.cpp
 *
 * \brief Contains the definition of the CertificateChecker methods.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CertificateChecker.h"

#include <QDate>
#include <QSslKey>

#include <openssl/dh.h>
#include <openssl/dsa.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(developermode)

bool CertificateChecker::checkCertificate(const QSslCertificate& pCertificate,
		QCryptographicHash::Algorithm pAlgorithm,
		const QSet<QString>& pAcceptedCertificateHashes)
{
	qDebug() << "Check certificate CN=" << pCertificate.subjectInfo(QSslCertificate::CommonName) << "SN=" << pCertificate.serialNumber();
	QString hash = QString::fromLatin1(pCertificate.digest(pAlgorithm).toHex());
	qDebug() << "Certificate hash(" << pAlgorithm << ")" << hash;
	qDebug() << "Accepted certificate hashes" << pAcceptedCertificateHashes;
	for (const auto& acceptedHash : pAcceptedCertificateHashes)
	{
		if (hash.compare(acceptedHash, Qt::CaseInsensitive) == 0)
		{
			return true;
		}
	}

	return false;
}


bool CertificateChecker::hasValidCertificateKeyLength(const QSslCertificate& pCertificate)
{
	auto keyLength = pCertificate.publicKey().length();
	auto keyAlgorithm = pCertificate.publicKey().algorithm();
	qDebug() << "Check certificate key of type" << CertificateChecker::toString(keyAlgorithm) << "and key size" << keyLength;
	return isValidKeyLength(keyLength, keyAlgorithm, false);
}


bool CertificateChecker::hasValidEphemeralKeyLength(const QSslKey& pEphemeralServerKey)
{
	int keyLength = pEphemeralServerKey.length();
	QSsl::KeyAlgorithm keyAlgorithm = pEphemeralServerKey.algorithm();

	if (keyAlgorithm == QSsl::Opaque)
	{
		// try do determine key algorithm and length
		if (auto key = static_cast<EVP_PKEY*>(pEphemeralServerKey.handle()))
		{
			keyLength = EVP_PKEY_bits(key);
			switch (EVP_PKEY_type(key->type))
			{
				case EVP_PKEY_RSA:
					keyAlgorithm = QSsl::Rsa;
					break;

				case EVP_PKEY_DSA:
				/* fall through */
				case EVP_PKEY_DH:
					keyAlgorithm = QSsl::Dsa;
					break;

				case EVP_PKEY_EC:
					keyAlgorithm = QSsl::Ec;
					break;
			}
		}
	}

	qDebug() << "Check ephemeral key of type" << CertificateChecker::toString(keyAlgorithm) << "and key size" << keyLength;
	return isValidKeyLength(keyLength, keyAlgorithm, true);
}


CertificateChecker::CertificateStatus CertificateChecker::checkAndSaveCertificate(const QSslCertificate& pCertificate, const QUrl& pUrl, QSharedPointer<AuthContext> pContext)
{
	Q_ASSERT(!pContext.isNull());

	if (!hasValidCertificateKeyLength(pCertificate))
	{
		return CertificateStatus::Unsupported_Algorithm_Or_Length;
	}

	// the call to cvc.isSyntaxValid is made to check, whether the cvc is set
	// TODO: do it more explicitly, e.g. implement a method cvc.isNull or so
	auto eac1 = pContext->getDidAuthenticateEac1();
	if (eac1 && pContext->getDvCvc())
	{
		if (auto certificateDescription = eac1->getCertificateDescription())
		{
			const QSet<QString> certHashes = certificateDescription->getCommCertificates();
			QCryptographicHash::Algorithm hashAlgo = pContext->getDvCvc()->getBody().getHashAlgorithm();
			if (!checkCertificate(pCertificate, hashAlgo, certHashes))
			{
				auto hashError = QStringLiteral("hash of certificate not in certificate description");

				if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
				{
					qCCritical(developermode) << hashError;
				}
				else
				{
					qCritical() << hashError;
					return CertificateStatus::Hash_Not_In_Description;
				}
			}
		}
	}

	pContext->addCertificateData(pUrl, pCertificate);
	return CertificateStatus::Good;
}


bool CertificateChecker::isValidKeyLength(int pKeyLength, QSsl::KeyAlgorithm pKeyAlgorithm, bool pIsEphemeral)
{
	const auto& secureStorage = AppSettings::getInstance().getSecureStorage();
	const int minKeySize = pIsEphemeral ? secureStorage.getMinimumEphemeralKeySize(pKeyAlgorithm) : secureStorage.getMinimumStaticKeySize(pKeyAlgorithm);

	qDebug() << "Minimum requested key size" << minKeySize;

	bool sufficient = pKeyLength >= minKeySize;
	if (!sufficient)
	{
		auto keySizeError = QStringLiteral("%1 key with insufficient key size found %2").arg(toString(pKeyAlgorithm)).arg(pKeyLength);
		if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
		{
			qCWarning(developermode) << keySizeError;
			sufficient = true;
		}
		else
		{
			qWarning() << keySizeError;
		}
	}
	return sufficient;
}


QString CertificateChecker::toString(QSsl::KeyAlgorithm pKeyAlgorithm)
{
	switch (pKeyAlgorithm)
	{
		case QSsl::KeyAlgorithm::Dsa:
			return QStringLiteral("Dsa");

		case QSsl::KeyAlgorithm::Rsa:
			return QStringLiteral("Rsa");

		case QSsl::KeyAlgorithm::Ec:
			return QStringLiteral("Ec");

		default:
			return QStringLiteral("Unknown (%1)").arg(pKeyAlgorithm);
	}
}
