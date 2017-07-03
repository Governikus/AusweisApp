/*!
 * TlsSettings.cpp
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "TlsSettings.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QMetaEnum>


using namespace governikus;

const QLatin1String SETTINGS_NAME_SSL_PROTOCOL_VERSION("protocolVersion");
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


TlsSettings::TlsSettings()
	: mConfiguration()
{
}


TlsSettings::~TlsSettings()
{
}


void TlsSettings::load(const QJsonObject& pConfig)
{
	const auto protocolVersion = readSslProtocol(pConfig, SETTINGS_NAME_SSL_PROTOCOL_VERSION);

	SslCipherList ciphers;
	QJsonArray pskCiphers;
	if (readJsonArray(pskCiphers, pConfig, SETTINGS_GROUP_NAME_CIPHERS))
	{
		for (const QJsonValue& line : qAsConst(pskCiphers))
		{
			ciphers += line.toString();
		}
	}

	SslEllipticCurveVector ellipticCurves;
	QJsonArray allowedEcs;
	if (pConfig.contains(SETTINGS_GROUP_NAME_ELLIPTIC_CURVES) && readJsonArray(allowedEcs, pConfig, SETTINGS_GROUP_NAME_ELLIPTIC_CURVES))
	{
		for (const QJsonValue& line : qAsConst(allowedEcs))
		{
			ellipticCurves += line.toString();
		}
	}

	const auto& signatureAlgorithms = readSignatureAlgorithms(pConfig, SETTINGS_GROUP_NAME_SIGNATURE_ALGORITHMS);

	mConfiguration = QSslConfiguration::defaultConfiguration();
	mConfiguration.setCaCertificates(QList<QSslCertificate>()); // disable fetching of system CA certificates. Set allowRootCertOnDemandLoading to false in Qt
	mConfiguration.setProtocol(protocolVersion);
	mConfiguration.setCiphers(ciphers);
	mConfiguration.setEllipticCurves(ellipticCurves);
	mConfiguration.setSignatureAndHashAlgorithms(signatureAlgorithms);
}


QSsl::SslProtocol TlsSettings::getProtocolVersion() const
{
	return mConfiguration.protocol();
}


QList<QSslCipher> TlsSettings::getCiphers() const
{
	return mConfiguration.ciphers();
}


QVector<QSslEllipticCurve> TlsSettings::getEllipticCurves() const
{
	return mConfiguration.ellipticCurves();
}


QVector<SignatureAlgorithmPair> TlsSettings::getSignatureAlgorithms() const
{
	return mConfiguration.signatureAndHashAlgorithms();
}


const QSslConfiguration& TlsSettings::getConfiguration() const
{
	return mConfiguration;
}


bool TlsSettings::readJsonArray(QJsonArray& pArray, const QJsonObject& pConfig, const QLatin1String& pName)
{
	QJsonValue value = pConfig.value(pName);
	if (value.isUndefined())
	{
		qDebug() << pName << "undefined, using default";
		return false;
	}
	if (!value.isArray())
	{
		qCritical() << "Expecting array for" << pName << "in SecureStorage";
		return false;
	}
	pArray = value.toArray();
	return true;
}


QSsl::SslProtocol TlsSettings::readSslProtocol(const QJsonObject& pConfig, const QLatin1String& pName)
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
		qCritical() << pName << ": Unsupported TLS protocol version detected" << value;
	}
	return QSsl::SslProtocol::SecureProtocols;
}


QVector<SignatureAlgorithmPair> TlsSettings::readSignatureAlgorithms(const QJsonObject& pConfig, const QLatin1String& pKey)
{
	const QJsonValue& tmp = pConfig[pKey];
	if (tmp.isUndefined())
	{
		qDebug() << pKey << "undefined, using default";
		return QVector<SignatureAlgorithmPair>();
	}
	if (!tmp.isArray())
	{
		qCritical() << pKey << "is malformed";
		return QVector<SignatureAlgorithmPair>();
	}
	const QJsonArray& array = tmp.toArray();

	QVector<SignatureAlgorithmPair> algorithms;
	for (const QJsonValue& line : array)
	{
		const auto& parts = line.toString().split(QLatin1Char('+'));
		if (parts.size() != 2)
		{
			qCritical() << pKey << "has malformed item" << line;
			return QVector<SignatureAlgorithmPair>();
		}

		static const auto& hashMetaEnum = QMetaEnum::fromType<QCryptographicHash::Algorithm>();
		bool hashConversionSuccessfull;
		const int hashInt = hashMetaEnum.keyToValue(parts[1].toLatin1().constData(), &hashConversionSuccessfull);
		if (!hashConversionSuccessfull)
		{
			qCritical() << "Not a hash algorithm" << parts[1];
			return QVector<SignatureAlgorithmPair>();
		}
		auto hash = static_cast<QCryptographicHash::Algorithm>(hashInt);

		if (parts[0] == QLatin1String("Rsa"))
		{
			algorithms += SignatureAlgorithmPair(QSsl::KeyAlgorithm::Rsa, hash);
		}
		else if (parts[0] == QLatin1String("Dsa"))
		{
			algorithms += SignatureAlgorithmPair(QSsl::KeyAlgorithm::Dsa, hash);
		}
		else if (parts[0] == QLatin1String("Ec"))
		{
			algorithms += SignatureAlgorithmPair(QSsl::KeyAlgorithm::Ec, hash);
		}
		else
		{
			qCritical() << "Not a signature algorithm" << parts[0];
			return QVector<SignatureAlgorithmPair>();
		}
	}
	return algorithms;
}
