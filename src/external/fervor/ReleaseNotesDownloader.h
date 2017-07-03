/*
 * \brief Helper to download release notes
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QUrl>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QScopedPointer>


namespace governikus
{

class ReleaseNotesDownloader : public QObject
{
	Q_OBJECT

	private:
		QEventLoop mEventLoop;
		bool mEncrypted;
		QString mReleaseNotes;
		QScopedPointer<QNetworkReply> mReply;

		void checkCertificate(const QSslCertificate& pPeerCert);

	private Q_SLOTS:
		void encrypted();
		void finished();
		void sslErrors(const QList<QSslError>& pErrors);

	public:
		ReleaseNotesDownloader();
		virtual ~ReleaseNotesDownloader();

		QString loadReleaseNotes(const QUrl& pUrl);
};

} /* namespace governikus */
