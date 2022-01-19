/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "TlsConfiguration.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QMetaEnum>

using namespace governikus;

const QLatin1String SETTINGS_NAME_SSL_PROTOCOL_VERSION("protocolVersion");
const QLatin1String SETTINGS_NAME_SSL_OCSP_STAPLING("ocspStapling");
const QLatin1String SETTINGS_GROUP_NAME_CIPHERS("ciphers");
const QLatin1String SETTINGS_GROUP_NAME_ELLIPTIC_CURVES("ellipticCurves");
const QLatin1String SETTINGS_GROUP_NAME_SIGNATURE_ALGORITHMS("signatureAlgorithms");


SslCipherList& SslCipherList::operator +=(const QString& pCipherName)
{
	QSslCipher cipher(pCipherName);
	if (cipher.isNull())
	{
		qWarning() << "Cipher is not supported by OpenSSL and will be ignored:" << pCipherName;
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
		qWarning() << "EllipticCurve is not supported by OpenSSL and will be ignored:" << pEllipticCurveName;
	}

	return *this;
}


TlsConfiguration::TlsConfiguration()
	: mConfiguration()
{
}


TlsConfiguration::~TlsConfiguration()
{
}


void TlsConfiguration::load(const QJsonObject& pConfig)
{
	const auto protocolVersion = readSslProtocol(pConfig, SETTINGS_NAME_SSL_PROTOCOL_VERSION);
	const auto ocspStapling = readOcspStapling(pConfig, SETTINGS_NAME_SSL_OCSP_STAPLING);

	SslCipherList ciphers;
	QJsonArray pskCiphers = readJsonArray(pConfig, SETTINGS_GROUP_NAME_CIPHERS);
	for (const QJsonValue& line : qAsConst(pskCiphers))
	{
		ciphers += line.toString();
	}

	SslEllipticCurveVector ellipticCurves;
	QJsonArray allowedEcs = readJsonArray(pConfig, SETTINGS_GROUP_NAME_ELLIPTIC_CURVES);
	for (const QJsonValue& line : qAsConst(allowedEcs))
	{
		ellipticCurves += line.toString();
	}

	const auto& signatureAlgorithms = readSignatureAlgorithms(pConfig, SETTINGS_GROUP_NAME_SIGNATURE_ALGORITHMS);

	mConfiguration = QSslConfiguration::defaultConfiguration();
	mConfiguration.setSslOption(QSsl::SslOptionDisableSessionPersistence, false); // enable SessionTicket, otherwise "sessionTicket()" returns empty data
	mConfiguration.setCaCertificates(QList<QSslCertificate>()); // disable fetching of system CA certificates. Set allowRootCertOnDemandLoading to false in Qt
	mConfiguration.setProtocol(protocolVersion);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
	mConfiguration.setOcspStaplingEnabled(ocspStapling);
#else
	Q_UNUSED(ocspStapling)
#endif
	mConfiguration.setCiphers(ciphers);
	mConfiguration.setEllipticCurves(ellipticCurves);
	mConfiguration.setBackendConfigurationOption(QByteArrayLiteral("SignatureAlgorithms"), signatureAlgorithms.join(':'));
}


QSsl::SslProtocol TlsConfiguration::getProtocolVersion() const
{
	return mConfiguration.protocol();
}


bool TlsConfiguration::getOcspStapling() const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
	return mConfiguration.ocspStaplingEnabled();

#else
	return false;

#endif
}


QList<QSslCipher> TlsConfiguration::getCiphers() const
{
	return mConfiguration.ciphers();
}


QVector<QSslEllipticCurve> TlsConfiguration::getEllipticCurves() const
{
	return mConfiguration.ellipticCurves();
}


QByteArrayList TlsConfiguration::getSignatureAlgorithms() const
{
	const auto signatureAlgorithms = mConfiguration.backendConfiguration().value(QByteArrayLiteral("SignatureAlgorithms")).toByteArray();
	if (!signatureAlgorithms.isEmpty())
	{
		return signatureAlgorithms.split(':');
	}

	return QByteArrayList();
}


const QSslConfiguration& TlsConfiguration::getConfiguration() const
{
	return mConfiguration;
}


QJsonArray TlsConfiguration::readJsonArray(const QJsonObject& pConfig, const QLatin1String pName) const
{
	QJsonValue value = pConfig.value(pName);
	if (value.isUndefined())
	{
		qDebug() << pName << "undefined, using default";
		return QJsonArray();
	}
	if (!value.isArray())
	{
		qCritical() << "Expecting array for" << pName << "in SecureStorage";
		return QJsonArray();
	}
	return value.toArray();
}


QSsl::SslProtocol TlsConfiguration::readSslProtocol(const QJsonObject& pConfig, const QLatin1String pName) const
{
	if (!pConfig.contains(pName))
	{
		qDebug() << pName << "undefined, using default";
	}
	else
	{
		const auto& value = pConfig.value(pName).toString();
		if (value == QLatin1String("TlsV1_0OrLater"))
		{
			return QSsl::SslProtocol::TlsV1_0OrLater;
		}
		if (value == QLatin1String("TlsV1_1OrLater"))
		{
			return QSsl::SslProtocol::TlsV1_1OrLater;
		}
		if (value == QLatin1String("TlsV1_2OrLater"))
		{
			return QSsl::SslProtocol::TlsV1_2OrLater;
		}
		if (value == QLatin1String("TlsV1_2"))
		{
			return QSsl::SslProtocol::TlsV1_2;
		}
		qCritical() << pName << ": Unsupported TLS protocol version detected" << value;
	}
	return QSsl::SslProtocol::SecureProtocols;
}


bool TlsConfiguration::readOcspStapling(const QJsonObject& pConfig, const QLatin1String pName) const
{
	if (!pConfig.contains(pName))
	{
		qDebug() << pName << "undefined, using default";
	}

	return pConfig.value(pName).toBool();
}


QByteArrayList TlsConfiguration::readSignatureAlgorithms(const QJsonObject& pConfig, const QLatin1String pKey) const
{
	const QJsonValue& tmp = pConfig[pKey];
	if (tmp.isUndefined())
	{
		qDebug() << pKey << "undefined, using default";
		return QByteArrayList();
	}
	if (!tmp.isArray())
	{
		qCritical() << pKey << "is malformed";
		return QByteArrayList();
	}
	const QJsonArray& array = tmp.toArray();

	QByteArrayList algorithms;
	for (const QJsonValue& line : array)
	{
		const auto& value = line.toString();
		if (value.count(QStringLiteral("+")) != 1)
		{
			qCritical() << pKey << "has malformed item" << line;
			return QByteArrayList();
		}

		algorithms += value.toUtf8();
	}

	return algorithms;
}
