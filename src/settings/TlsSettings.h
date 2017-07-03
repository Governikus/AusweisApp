/*!
 * TlsSettings.h
 *
 * \brief Configuration options for TLS channels
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractSettings.h"

#include <QCryptographicHash>
#include <QJsonObject>
#include <QList>
#include <QSslCipher>
#include <QSslConfiguration>
#include <QSslEllipticCurve>
#include <QStringList>
#include <QVector>


class test_TlsSettings;


namespace governikus
{


class SecureStorage;
using SignatureAlgorithmPair = QPair<QSsl::KeyAlgorithm, QCryptographicHash::Algorithm>;


class SslCipherList
	: public QList<QSslCipher>
{
	public:
		SslCipherList& operator +=(const QString& pCipherName);
};


class SslEllipticCurveVector
	: public QVector<QSslEllipticCurve>
{
	public:
		SslEllipticCurveVector& operator +=(const QString& pEllipticCurveName);
};


class TlsSettings final
{
	friend class SecureStorage;
	friend class::test_TlsSettings;
	friend bool operator==(const TlsSettings& pLeft, const TlsSettings& pRight);

	private:
		QSslConfiguration mConfiguration;

		TlsSettings();
		~TlsSettings();

		bool readJsonArray(QJsonArray& pArray, const QJsonObject& pConfig, const QLatin1String& pName);
		QString readGroup(const QJsonObject& pConfig, const QLatin1String& pGroup, const QLatin1String& pName);
		QSsl::SslProtocol readSslProtocol(const QJsonObject& pConfig, const QLatin1String& pName);
		QVector<SignatureAlgorithmPair> readSignatureAlgorithms(const QJsonObject& pConfig, const QLatin1String& pKey);

	public:
		void load(const QJsonObject& pConfig);

		QSsl::SslProtocol getProtocolVersion() const;
		QList<QSslCipher> getCiphers() const;
		QVector<QSslEllipticCurve> getEllipticCurves() const;
		QVector<SignatureAlgorithmPair> getSignatureAlgorithms() const;
		const QSslConfiguration& getConfiguration() const;
};


inline bool operator==(const TlsSettings& pLeft, const TlsSettings& pRight)
{
	return &pLeft == &pRight || (
		pLeft.mConfiguration == pRight.mConfiguration);
}


inline bool operator!=(const TlsSettings& pLeft, const TlsSettings& pRight)
{
	return !(pLeft == pRight);
}


} // namespace governikus
