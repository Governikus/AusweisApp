/*!
 * \brief Generic class that allows to download files from a server to the
 *        local application cache.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"
#include "NetworkManager.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QQueue>
#include <QSharedPointer>
#include <QSslCipher>
#include <QUrl>

namespace governikus
{
class Downloader
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<QNetworkRequest> mCurrentRequest;
		QNetworkReply* mCurrentReply;
		QQueue<QSharedPointer<QNetworkRequest> > mPendingRequests;

		void scheduleDownload(QSharedPointer<QNetworkRequest> pDownloadRequest);
		void startDownloadIfPending();

	protected:
		Downloader();
		virtual ~Downloader();

	private Q_SLOTS:
		void onSslErrors(const QList<QSslError>& pErrors);
		void onSslHandshakeDone();
		void onMetadataChanged();
		void onNetworkReplyFinished();

	public:
		Q_INVOKABLE virtual void download(const QUrl& pUpdateUrl);
		Q_INVOKABLE virtual void downloadIfNew(const QUrl& pUpdateUrl,
				const QDateTime& pCurrentTimestamp);

		static Downloader& getInstance();

	Q_SIGNALS:
		void fireDownloadSuccess(const QUrl& pUpdateUrl, const QDateTime& pNewTimestamp, const QByteArray& pData);
		void fireDownloadFailed(const QUrl& pUpdateUrl, GlobalStatus::Code pErrorCode);
		void fireDownloadUnnecessary(const QUrl& pUpdateUrl);
};

} /* namespace governikus */
