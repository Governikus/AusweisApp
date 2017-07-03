/*!
 * CertificateChecker.cpp
 *
 * \brief Contains the definition of the CertificateChecker methods.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CertificateChecker.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
#include "HashAlgorithmUtil.h"
#endif

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


bool CertificateChecker::isValidKeyLength(int pKeyLength, QSsl::KeyAlgorithm pKeyAlgorithm, bool pIsEphemeral)
{
	switch (pKeyAlgorithm)
	{
		case QSsl::KeyAlgorithm::Rsa:
		{
			bool sufficient = pKeyLength >= 2000;
			if (!sufficient)
			{
				auto keySizeError = QStringLiteral("RSA key with insufficient key size found %1").arg(pKeyLength);
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

		case QSsl::KeyAlgorithm::Dsa:
		{
			bool sufficient = (pIsEphemeral ? pKeyLength >= 1024 : pKeyLength >= 2000);
			if (!sufficient)
			{
				auto keySizeError = QStringLiteral("DSA key with insufficient key size found %1").arg(pKeyLength);
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

		case QSsl::KeyAlgorithm::Ec:
		{
			bool sufficient = pKeyLength >= 224;
			if (!sufficient)
			{
				auto keySizeError = QStringLiteral("EC key with insufficient key size found %1").arg(pKeyLength);
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

		default:
			auto algorithmError = QStringLiteral("Unknown algorithm used");
			if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
			{
				qCWarning(developermode) << algorithmError;
			}
			else
			{
				qWarning() << algorithmError;
			}
	}

	return false;
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
