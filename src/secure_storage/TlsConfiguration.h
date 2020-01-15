/*!
 * \brief Configuration options for TLS channels
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArrayList>
#include <QCryptographicHash>
#include <QJsonObject>
#include <QList>
#include <QSslCipher>
#include <QSslConfiguration>
#include <QSslEllipticCurve>
#include <QStringList>
#include <QVector>


class test_TlsConfiguration;


namespace governikus
{

class SecureStorage;


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


class TlsConfiguration final
{
	friend class SecureStorage;
	friend class ::test_TlsConfiguration;
	friend bool operator==(const TlsConfiguration& pLeft, const TlsConfiguration& pRight);

	private:
		QSslConfiguration mConfiguration;

		TlsConfiguration();
		~TlsConfiguration();

		bool readJsonArray(QJsonArray& pArray, const QJsonObject& pConfig, const QLatin1String pName);
		QSsl::SslProtocol readSslProtocol(const QJsonObject& pConfig, const QLatin1String pName);
		QByteArrayList readSignatureAlgorithms(const QJsonObject& pConfig, const QLatin1String pKey);

	public:
		void load(const QJsonObject& pConfig);

		QSsl::SslProtocol getProtocolVersion() const;
		QList<QSslCipher> getCiphers() const;
		QVector<QSslEllipticCurve> getEllipticCurves() const;
		QByteArrayList getSignatureAlgorithms() const;
		const QSslConfiguration& getConfiguration() const;
};


inline bool operator==(const TlsConfiguration& pLeft, const TlsConfiguration& pRight)
{
	return &pLeft == &pRight || (
		pLeft.mConfiguration == pRight.mConfiguration);
}


inline bool operator!=(const TlsConfiguration& pLeft, const TlsConfiguration& pRight)
{
	return !(pLeft == pRight);
}


} // namespace governikus
