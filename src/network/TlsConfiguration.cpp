/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"
#include "TlsConfiguration.h"

#include <QDate>
#include <QLoggingCategory>
#include <QStringList>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(developermode)
Q_DECLARE_LOGGING_CATEGORY(network)

QSslConfiguration TlsConfiguration::mConfig;
QSslConfiguration TlsConfiguration::mPskConfig;

SslEllipticCurveVector TlsConfiguration::mEllipticCurves;
SslCipherList TlsConfiguration::mPskCiphers;
SslCipherList TlsConfiguration::mCiphersWithForwardSecrecy;
SslCipherList TlsConfiguration::mCiphersWithSha1ForBackwardCompatibility;


SslCipherList& SslCipherList::operator +=(const QString& pCipherName)
{
	QSslCipher cipher(pCipherName);
	if (cipher.isNull())
	{
		qCWarning(network) << "Cipher is not supported by OpenSSL and will be ignored:" << pCipherName;
	}
	else
	{
		append(cipher);
	}

	return *this;
}


SslEllipticCurveVector& SslEllipticCurveVector::operator +=(const QString& pEllipticCurveName)
{
	QSslEllipticCurve curve = QSslEllipticCurve::fromLongName(pEllipticCurveName);
	if (curve.isValid() && curve.isTlsNamedCurve())
	{
		append(curve);
	}
	else
	{
		qCWarning(network) << "EllipticCurve is not supported by OpenSSL and will be ignored:" << pEllipticCurveName;
	}

	return *this;
}


const QVector<QSslEllipticCurve>& TlsConfiguration::getAllowedSslEllipticCurves()
{
	if (mEllipticCurves.isEmpty())
	{
		for (const QString& cipherName : AppSettings::getInstance().getSecureStorage().getAllowedSslEllipticCurves())
		{
			mEllipticCurves += cipherName;
		}
	}
	return mEllipticCurves;
}


const QList<QSslCipher>& TlsConfiguration::getCiphersWithPsk()
{
	if (mPskCiphers.isEmpty())
	{
		for (const QString& cipherName : AppSettings::getInstance().getSecureStorage().getCiphersWithPsk())
		{
			mPskCiphers += cipherName;
		}
	}
	return mPskCiphers;
}


/*!
 * This is the intersection of all ciphers with forward secrecy needed by TR-03116 Part 4
 * and all ciphers supported by openSSL according to https://www.openssl.org/docs/apps/ciphers.html.
 */
const QList<QSslCipher>& TlsConfiguration::getCiphersWithForwardSecrecy()
{
	if (mCiphersWithForwardSecrecy.isEmpty())
	{
		for (const QString& cipherName : AppSettings::getInstance().getSecureStorage().getCiphersWithForwardSecrecy())
		{
			mCiphersWithForwardSecrecy += cipherName;
		}
	}
	return mCiphersWithForwardSecrecy;
}


/*!
 * Enable SHA1 for message authentication as a "compatibility exception", see TR-02102-2, section 3.3.2
 */
const QList<QSslCipher>& TlsConfiguration::getCiphersWithSha1ForBackwardCompatibility()
{
	if (mCiphersWithSha1ForBackwardCompatibility.isEmpty())
	{
		for (const QString& cipherName : AppSettings::getInstance().getSecureStorage().getCiphersWithSha1ForBackwardCompatibility())
		{
			mCiphersWithSha1ForBackwardCompatibility += cipherName;
		}
	}
	return mCiphersWithSha1ForBackwardCompatibility;
}


QSslConfiguration TlsConfiguration::createSslConfiguration()
{
	if (!mConfig.isNull())
	{
		return mConfig;
	}

	QSslConfiguration config(QSslConfiguration::defaultConfiguration());

	QList<QSslCipher> allowedCiphers = getCiphersWithForwardSecrecy();
	allowedCiphers.append(getCiphersWithSha1ForBackwardCompatibility());

	// disable system CA certificates. Set allowRootCertOnDemandLoading = false
	config.setCaCertificates(QSslConfiguration::systemCaCertificates());
	config.setCiphers(allowedCiphers);
	config.setProtocol(AppSettings::getInstance().getSecureStorage().getSslProtocolVersion());
	config.setEllipticCurves(getAllowedSslEllipticCurves());
	config.setSignatureAndHashAlgorithms(AppSettings::getInstance().getSecureStorage().getSignatureAlgorithms());
	mConfig = config;
	return mConfig;
}


QSslConfiguration TlsConfiguration::createPskSslConfiguration()
{
	if (!mPskConfig.isNull())
	{
		return mPskConfig;
	}

	QSslConfiguration config(QSslConfiguration::defaultConfiguration());

	// disable system CA certificates. Set allowRootCertOnDemandLoading = false
	config.setCaCertificates(QSslConfiguration::systemCaCertificates());
	config.setCiphers(getCiphersWithPsk());
	config.setProtocol(AppSettings::getInstance().getSecureStorage().getSslProtocolVersionPsk());
	config.setSignatureAndHashAlgorithms(AppSettings::getInstance().getSecureStorage().getSignatureAlgorithmsPsk());
	mPskConfig = config;
	return mPskConfig;
}


QStringList TlsConfiguration::getFatalErrors(const QList<QSslError>& pErrors)
{
	static const QSet<QSslError::SslError> fatalErrors(
		{QSslError::CertificateSignatureFailed, QSslError::CertificateNotYetValid, QSslError::CertificateExpired,
		 QSslError::InvalidNotBeforeField, QSslError::InvalidNotAfterField, QSslError::CertificateRevoked, QSslError::InvalidCaCertificate,
		 QSslError::CertificateRejected, QSslError::SubjectIssuerMismatch, QSslError::HostNameMismatch,
		 QSslError::UnspecifiedError, QSslError::NoSslSupport, QSslError::CertificateBlacklisted}
		);

	QStringList fatalErrorStrings;
	for (const auto& error : pErrors)
	{
		QString msg = QStringLiteral("%1: %2").arg(static_cast<int>(error.error())).arg(error.errorString());
		if (fatalErrors.contains(error.error()))
		{
			if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
			{
				qCWarning(developermode) << msg;
			}
			else
			{
				qCWarning(network) << msg;
				fatalErrorStrings += msg;
			}
		}
		else
		{
			qCWarning(network) << "(ignored) " << msg;
		}
	}
	return fatalErrorStrings;
}


bool TlsConfiguration::containsFatalError(const QList<QSslError>& pErrors)
{
	return !getFatalErrors(pErrors).isEmpty();
}
