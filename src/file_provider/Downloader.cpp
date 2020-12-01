/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "Downloader.h"

#include "LogHandler.h"
#include "TlsChecker.h"

#include <http_parser.h>
#include <QFile>
#include <QLocale>
#include <QLoggingCategory>
#include <QMutableListIterator>
#include <QScopeGuard>

Q_DECLARE_LOGGING_CATEGORY(network)
Q_DECLARE_LOGGING_CATEGORY(fileprovider)

using namespace governikus;


static const char* const cABORTED = "aborted_download";

void Downloader::scheduleDownload(QSharedPointer<QNetworkRequest> request)
{
	mPendingRequests.enqueue(request);

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

	mCurrentReply.reset(Env::getSingleton<NetworkManager>()->get(*mPendingRequests.dequeue()), &QObject::deleteLater);

	connect(mCurrentReply.data(), &QNetworkReply::sslErrors, this, &Downloader::onSslErrors);
	connect(mCurrentReply.data(), &QNetworkReply::encrypted, this, &Downloader::onSslHandshakeDone);
	connect(mCurrentReply.data(), &QNetworkReply::metaDataChanged, this, &Downloader::onMetadataChanged);
	connect(mCurrentReply.data(), &QNetworkReply::finished, this, &Downloader::onNetworkReplyFinished);
	connect(mCurrentReply.data(), &QNetworkReply::downloadProgress, this, &Downloader::onNetworkReplyProgress);
}


void Downloader::onSslErrors(const QList<QSslError>& pErrors)
{
	TlsChecker::containsFatalError(mCurrentReply, pErrors);
}


void Downloader::onSslHandshakeDone()
{
	const auto& cfg = mCurrentReply->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(network));

	if (!Env::getSingleton<NetworkManager>()->checkUpdateServerCertificate(mCurrentReply))
	{
		const QString& textForLog = mCurrentReply->request().url().fileName();
		qCCritical(fileprovider).nospace() << "Untrusted certificate found [" << textForLog << "]: " << cfg.peerCertificate();
		mCurrentReply->abort();
	}
}


void Downloader::onMetadataChanged()
{
	const QString& fileName = mCurrentReply->request().url().fileName();

	const auto statusCode = mCurrentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == HTTP_STATUS_OK)
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
				mCurrentReply.reset();
				startDownloadIfPending();
			});

	const QUrl url = mCurrentReply->request().url();
	const QString& textForLog = url.fileName();
	if (!Env::getSingleton<NetworkManager>()->checkUpdateServerCertificate(mCurrentReply))
	{
		qCCritical(fileprovider).nospace() << "Connection not secure [" << textForLog << "]";
		Q_EMIT fireDownloadFailed(url, GlobalStatus::Code::Network_Ssl_Establishment_Error);

		return;
	}

	if (mCurrentReply->property(cABORTED).toBool())
	{
		qCCritical(fileprovider) << "Download aborted...";
		Q_EMIT fireDownloadFailed(url, GlobalStatus::Code::Downloader_Aborted);
		return;
	}

	const auto hasError = mCurrentReply->error() != QNetworkReply::NoError;
	const auto statusCode = NetworkManager::getLoggedStatusCode(mCurrentReply, spawnMessageLogger(network));
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

			const auto readData = mCurrentReply->readAll();
			if (!hasError && readData.size() > 0)
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

	QMutableListIterator<QSharedPointer<QNetworkRequest> > iterator(mPendingRequests);
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


void Downloader::download(const QUrl& pUpdateUrl)
{
	qCDebug(fileprovider) << "Download:" << pUpdateUrl;
	auto request = QSharedPointer<QNetworkRequest>::create(pUpdateUrl);
	scheduleDownload(request);
}


void Downloader::downloadIfNew(const QUrl& pUpdateUrl,
		const QDateTime& pCurrentTimestamp)
{
	qCDebug(fileprovider) << "Download:" << pUpdateUrl;
	auto request = QSharedPointer<QNetworkRequest>::create(pUpdateUrl);
	request->setHeader(QNetworkRequest::IfModifiedSinceHeader, pCurrentTimestamp);
	scheduleDownload(request);
}
