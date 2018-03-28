/*!
 * \brief RemoteService settings
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractSettings.h"

#include <QDateTime>
#include <QList>
#include <QSet>
#include <QSslCertificate>
#include <QSslKey>
#include <QString>
#include <QVector>

class test_RemoteServiceSettings;
class test_RemoteConnector;
class test_RemoteTlsServer;
class test_RemoteWebSocketServer;

namespace governikus
{

class RemoteServiceSettings
	: public AbstractSettings
{
	Q_OBJECT

	friend class AppSettings;
	friend class ::test_RemoteServiceSettings;
	friend class ::test_RemoteConnector;
	friend class ::test_RemoteTlsServer;
	friend class ::test_RemoteWebSocketServer;

	public:
		class RemoteInfo
		{
			friend class RemoteServiceSettings;
			friend class ::test_RemoteServiceSettings;

			private:
				QString mFingerprint;
				QString mName;
				QDateTime mLastConnected;

				RemoteInfo(const QString& pFingerprint,
						const QDateTime& pLastConnected = QDateTime(),
						const QString& pName = QString());

			public:
				RemoteInfo() = default;

				const QString& getFingerprint() const;

				const QString& getName() const;
				void setName(const QString& pName);

				const QDateTime& getLastConnected() const;
				void setLastConnected(const QDateTime& pLastConnected);

				bool operator==(const RemoteInfo& pOther) const;
				bool operator!=(const RemoteInfo& pOther) const;
		};

	private:
		QSharedPointer<QSettings> mStore;

		RemoteServiceSettings();
		QString getDefaultServerName();
		void setTrustedCertificates(const QList<QSslCertificate>& pCertificates);
		void setUniqueTrustedCertificates(const QSet<QSslCertificate>& pCertificates);

		void setRemoteInfos(const QVector<RemoteInfo>& pInfos);
		void syncRemoteInfos(const QSet<QSslCertificate>& pCertificates);

	public:
		static QString generateFingerprint(const QSslCertificate& pCert);
		virtual ~RemoteServiceSettings() override;
		virtual void save() override;

		QString getServerName() const;
		void setServerName(const QString& pName);

		bool getPinPadMode() const;
		void setPinPadMode(bool pPinPadMode);

		QList<QSslCertificate> getTrustedCertificates() const;
		void addTrustedCertificate(const QSslCertificate& pCertificate);
		void removeTrustedCertificate(const QSslCertificate& pCertificate);
		void removeTrustedCertificate(const QString& pFingerprint);

		QSslCertificate getCertificate() const;
		void setCertificate(const QSslCertificate& pCert) const;

		QSslKey getKey() const;
		void setKey(const QSslKey& pKey) const;

		RemoteInfo getRemoteInfo(const QSslCertificate& pCertificate) const;
		RemoteInfo getRemoteInfo(const QString& pFingerprint) const;
		QVector<RemoteInfo> getRemoteInfos() const;
		bool updateRemoteInfo(const RemoteInfo& pInfo);

	Q_SIGNALS:
		void fireTrustedCertificatesChanged();
		void fireTrustedRemoteInfosChanged();
};


} /* namespace governikus */

Q_DECLARE_TYPEINFO(governikus::RemoteServiceSettings::RemoteInfo, Q_MOVABLE_TYPE);
