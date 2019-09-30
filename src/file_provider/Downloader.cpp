/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "Downloader.h"

#include "LogHandler.h"
#include "SingletonHelper.h"
#include "TlsChecker.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#include <QScopeGuard>
#else
#include "ScopeGuard.h"
#endif

#include <http_parser.h>
#include <QFile>
#include <QLocale>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(network)
Q_DECLARE_LOGGING_CATEGORY(fileprovider)

using namespace governikus;


defineSingleton(Downloader)


Downloader & Downloader::getInstance()
{
	return *Instance;
}


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

	mCurrentRequest = mPendingRequests.dequeue();
	mCurrentReply = Env::getSingleton<NetworkManager>()->get(*mCurrentRequest);

	connect(mCurrentReply, &QNetworkReply::sslErrors, this, &Downloader::onSslErrors);
	connect(mCurrentReply, &QNetworkReply::encrypted, this, &Downloader::onSslHandshakeDone);
	connect(mCurrentReply, &QNetworkReply::metaDataChanged, this, &Downloader::onMetadataChanged);
	connect(mCurrentReply, &QNetworkReply::finished, this, &Downloader::onNetworkReplyFinished);
}


void Downloader::onSslErrors(const QList<QSslError>& pErrors)
{
	TlsChecker::containsFatalError(mCurrentReply, pErrors);
}


void Downloader::onSslHandshakeDone()
{
	const auto& cfg = mCurrentReply->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(network));

	if (!Env::getSingleton<NetworkManager>()->checkUpdateServerCertificate(*mCurrentReply))
	{
		const QString& textForLog = mCurrentRequest->url().fileName();
		qCCritical(fileprovider).nospace() << "Untrusted certificate found [" << textForLog << "]: " << cfg.peerCertificate();
		mCurrentReply->abort();
	}
}


void Downloader::onMetadataChanged()
{
	const QString& fileName = mCurrentRequest->url().fileName();

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
				mCurrentReply->deleteLater();
				mCurrentReply = nullptr;
				startDownloadIfPending();
			});

	if (mCurrentRequest.isNull())
	{
		qCCritical(fileprovider) << "Internal error: no running download request.";
		Q_EMIT fireDownloadFailed(mCurrentRequest->url(), GlobalStatus::Code::Network_Other_Error);

		return;
	}

	const QUrl url = mCurrentRequest->url();
	const QString& textForLog = mCurrentRequest->url().fileName();
	if (!Env::getSingleton<NetworkManager>()->checkUpdateServerCertificate(*mCurrentReply))
	{
		qCCritical(fileprovider).nospace() << "Connection not secure [" << textForLog << "]";
		Q_EMIT fireDownloadFailed(url, GlobalStatus::Code::Network_Ssl_Establishment_Error);

		return;
	}

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

			Q_EMIT fireDownloadSuccess(mCurrentRequest->url(), lastModified, mCurrentReply->readAll());
			break;
		}

		case HTTP_STATUS_NOT_MODIFIED:
			Q_EMIT fireDownloadUnnecessary(url);
			break;

		case HTTP_STATUS_NOT_FOUND:
			Q_EMIT fireDownloadFailed(url, GlobalStatus::Code::Downloader_File_Not_Found);
			break;

		default:
			if (mCurrentReply->error() != QNetworkReply::NoError)
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


Downloader::Downloader()
	: mCurrentRequest()
	, mCurrentReply(nullptr)
	, mPendingRequests()
{
}


Downloader::~Downloader()
{
	if (mCurrentReply != nullptr)
	{
		if (mCurrentReply->isRunning() && !mCurrentRequest.isNull())
		{
			const QString& textForLog = mCurrentRequest->url().fileName();
			qCDebug(fileprovider).nospace() << "Scheduling pending update request [" << textForLog << "] for deletion";
		}
		mCurrentReply->deleteLater();
		mCurrentReply = nullptr;
	}
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
	const QString& timeStampString = QLocale::c().toString(pCurrentTimestamp, QStringLiteral("ddd, dd MMM yyyy hh:mm:ss 'GMT'"));
	if (!timeStampString.isEmpty())
	{
		request->setRawHeader(QByteArrayLiteral("If-Modified-Since"), timeStampString.toLatin1());
	}
	// See Section 14.25 at https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
	// Example timestamp string:      Sun, 06 Nov 1994 08:49:37 GMT
	scheduleDownload(request);
}
