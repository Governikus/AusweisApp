/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "GlobalStatus.h"

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
	friend class Env;

	private:
		QSharedPointer<QNetworkReply> mCurrentReply;
		QQueue<QNetworkRequest> mPendingRequests;

		void scheduleDownload(const QNetworkRequest& pDownloadRequest);
		void startDownloadIfPending();

	protected:
		Downloader();
		~Downloader() override;

	private Q_SLOTS:
		void onMetadataChanged();
		void onNetworkReplyFinished();
		void onNetworkReplyProgress(qint64 pBytesReceived, qint64 pBytesTotal);

	public:
		bool abort(const QUrl& pUpdateUrl);
		virtual void download(const QUrl& pUpdateUrl, const QDateTime& pCurrentTimestamp = QDateTime());

	Q_SIGNALS:
		void fireDownloadProgress(const QUrl& pUpdateUrl, qint64 pBytesReceived, qint64 pBytesTotal);
		void fireDownloadSuccess(const QUrl& pUpdateUrl, const QDateTime& pNewTimestamp, const QByteArray& pData);
		void fireDownloadFailed(const QUrl& pUpdateUrl, GlobalStatus::Code pErrorCode);
		void fireDownloadUnnecessary(const QUrl& pUpdateUrl);
};

} // namespace governikus
