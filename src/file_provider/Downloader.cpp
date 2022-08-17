/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "Downloader.h"

#include "LogHandler.h"

#include <QFile>
#include <QLocale>
#include <QLoggingCategory>
#include <QMutableListIterator>
#include <QScopeGuard>
#include <http_parser.h>

Q_DECLARE_LOGGING_CATEGORY(network)
Q_DECLARE_LOGGING_CATEGORY(fileprovider)

using namespace governikus;


static const char* const cABORTED = "aborted_download";

void Downloader::scheduleDownload(QSharedPointer<QNetworkRequest> pDownloadRequest)
{
	mPendingRequests.enqueue(pDownloadRequest);

	startDownloadIfPending();
}


void Downloader::startDownloadIfPending()
{
	if (mCurrentReply)
	{
		qCDebug(fileprovider) << "A download is already in progress... delaying.";
		return;
	}

	if (mPendingRequests.isEmpty())
	{
		qCDebug(fileprovider) << "No pending requests to be started.";
		return;
	}

	mCurrentReply = Env::getSingleton<NetworkManager>()->getAsUpdater(*mPendingRequests.dequeue());

	connect(mCurrentReply.data(), &QNetworkReply::metaDataChanged, this, &Downloader::onMetadataChanged);
	connect(mCurrentReply.data(), &QNetworkReply::finished, this, &Downloader::onNetworkReplyFinished);
	connect(mCurrentReply.data(), &QNetworkReply::downloadProgress, this, &Downloader::onNetworkReplyProgress);
}


void Downloader::onMetadataChanged()
{
	const QString& fileName = mCurrentReply->request().url().fileName();

	if (mCurrentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == HTTP_STATUS_OK)
	{
		qCDebug(fileprovider) << "Continue request for" << fileName;
		return;
	}

	qCDebug(fileprovider) << "Abort request for" << fileName;
	mCurrentReply->abort();
}


void Downloader::onNetworkReplyFinished()
{
	qCDebug(fileprovider) << "Downloader finished:" << mCurrentReply->request().url().fileName();

	const auto guard = qScopeGuard([this] {
			mCurrentReply->disconnect(this);
			mCurrentReply.reset();
			startDownloadIfPending();
		});

	const QUrl url = mCurrentReply->request().url();
	if (mCurrentReply->property(cABORTED).toBool())
	{
		qCCritical(fileprovider) << "Download aborted...";
		Q_EMIT fireDownloadFailed(url, GlobalStatus::Code::Downloader_Aborted);
		return;
	}

	const auto hasError = mCurrentReply->error() != QNetworkReply::NoError;
	const auto statusCode = NetworkManager::getLoggedStatusCode(mCurrentReply, spawnMessageLogger(network));
	const QString& textForLog = url.fileName();
	switch (statusCode)
	{
		case HTTP_STATUS_OK:
		{
			QDateTime lastModified = mCurrentReply->header(QNetworkRequest::KnownHeaders::LastModifiedHeader).toDateTime();
			if (!lastModified.isValid())
			{
				qCWarning(fileprovider) << "Server did not provide a valid LastModifiedHeader";
				lastModified = QDateTime::currentDateTime();
			}

			if (const auto& readData = mCurrentReply->readAll(); !hasError && readData.size() > 0)
			{
				Q_EMIT fireDownloadSuccess(url, lastModified, readData);
			}
			else
			{
				qCCritical(fileprovider).nospace() << "Received no data." << mCurrentReply->errorString() << " [" << textForLog << "]";
				Q_EMIT fireDownloadFailed(url, NetworkManager::toStatus(mCurrentReply).getStatusCode());
			}
			break;
		}

		case HTTP_STATUS_NOT_MODIFIED:
			Q_EMIT fireDownloadUnnecessary(url);
			break;

		case HTTP_STATUS_NOT_FOUND:
			Q_EMIT fireDownloadFailed(url, GlobalStatus::Code::Downloader_File_Not_Found);
			break;

		default:
			if (hasError)
			{
				qCCritical(fileprovider).nospace() << mCurrentReply->errorString() << " [" << textForLog << "]";
				Q_EMIT fireDownloadFailed(url, NetworkManager::toStatus(mCurrentReply).getStatusCode());
			}
			else
			{
				qCCritical(fileprovider).nospace() << "Invalid HTTP status code for [" << textForLog << "]";
				Q_EMIT fireDownloadFailed(url, GlobalStatus::Code::Network_Other_Error);
			}
	}
}


void Downloader::onNetworkReplyProgress(qint64 pBytesReceived, qint64 pBytesTotal)
{
	Q_EMIT fireDownloadProgress(mCurrentReply->request().url(), pBytesReceived, pBytesTotal);
}


Downloader::Downloader()
	: mCurrentReply()
	, mPendingRequests()
{
}


Downloader::~Downloader()
{
	if (!mCurrentReply.isNull() && mCurrentReply->isRunning())
	{
		const QString& textForLog = mCurrentReply->request().url().fileName();
		qCDebug(fileprovider).nospace() << "Scheduling pending update request [" << textForLog << "] for deletion";
	}
}


bool Downloader::abort(const QUrl& pUpdateUrl)
{
	bool aborted = false;
	qCDebug(fileprovider) << "Try abort of download:" << pUpdateUrl;

	if (!mCurrentReply.isNull() && mCurrentReply->isRunning() && mCurrentReply->request().url() == pUpdateUrl)
	{
		mCurrentReply->setProperty(cABORTED, QVariant(true));
		mCurrentReply->abort();
		qCDebug(fileprovider) << "Current download aborted";
		aborted = true;
	}

	QMutableListIterator<QSharedPointer<QNetworkRequest>> iterator(mPendingRequests);
	while (iterator.hasNext())
	{
		const auto item = iterator.next();
		if (item->url() == pUpdateUrl)
		{
			qCDebug(fileprovider) << "Remove pending request";

			Q_EMIT fireDownloadFailed(pUpdateUrl, GlobalStatus::Code::Downloader_Aborted);
			aborted = true;
			iterator.remove();
		}
	}

	return aborted;
}


void Downloader::download(const QUrl& pUpdateUrl, const QDateTime& pCurrentTimestamp)
{
	QMetaObject::invokeMethod(this, [this, pUpdateUrl, pCurrentTimestamp] {
			qCDebug(fileprovider) << "Download:" << pUpdateUrl;
			auto request = QSharedPointer<QNetworkRequest>::create(pUpdateUrl);
			if (pCurrentTimestamp.isValid())
			{
				request->setHeader(QNetworkRequest::IfModifiedSinceHeader, pCurrentTimestamp);
			}
			scheduleDownload(request);
		});
}
