/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Configuration options for TLS channels
 */

#pragma once

#include <QByteArrayList>
#include <QJsonObject>
#include <QList>
#include <QSslCipher>
#include <QSslConfiguration>
#include <QSslEllipticCurve>
#include <QStringList>


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
	: public QList<QSslEllipticCurve>
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
		~TlsConfiguration() = default;

		[[nodiscard]] QJsonArray readJsonArray(const QJsonObject& pConfig, const QLatin1String pName) const;
		[[nodiscard]] QSsl::SslProtocol readSslProtocol(const QJsonObject& pConfig, const QLatin1String pName) const;
		[[nodiscard]] bool readOcspStapling(const QJsonObject& pConfig, const QLatin1String pName) const;
		[[nodiscard]] QByteArrayList readSignatureAlgorithms(const QJsonObject& pConfig, const QLatin1String pKey) const;

	public:
		void load(const QJsonObject& pConfig);
		[[nodiscard]] bool isValid() const;

		[[nodiscard]] QSsl::SslProtocol getProtocolVersion() const;
		[[nodiscard]] bool getOcspStapling() const;
		[[nodiscard]] QList<QSslCipher> getCiphers() const;
		[[nodiscard]] QList<QSslEllipticCurve> getEllipticCurves() const;
		[[nodiscard]] QByteArrayList getSignatureAlgorithms() const;
		[[nodiscard]] const QSslConfiguration& getConfiguration() const;
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
