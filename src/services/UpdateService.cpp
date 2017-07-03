/*!
 * UpdateService.cpp
 *
 * \brief Generic implementation of a service providing an update mechanism for settings,
 *        images, and so on.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "UpdateService.h"

#include "HttpStatusCode.h"
#include "NetworkManager.h"
#include "TlsConfiguration.h"

#include <QLoggingCategory>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(update)


void UpdateService::onSslErrors(const QList<QSslError>& pErrors)
{
	TlsConfiguration::containsFatalError(mReply, pErrors);
}


void UpdateService::onSslHandshakeDone()
{
	const auto& cfg = mReply->sslConfiguration();
	const auto& cert = cfg.peerCertificate();
	qDebug(update) << "Used session cipher:" << cfg.sessionCipher();
	qDebug(update) << "Used session protocol:" << cfg.sessionProtocol();
	qDebug(update) << "Used server certificate:" << cert;

	if (!mTrustedUpdateCertificates.contains(cert))
	{
		qCritical(update).nospace() << "Untrusted certificate found [" << mNameForLog << "]: " << cert;
		mReply->abort();
	}
}


void UpdateService::onMetadataChanged()
{
	qCDebug(update) << "Meta data changed for" << mNameForLog;

	QVariant status = mReply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute);
	if (!status.isNull())
	{
		qCDebug(update) << "Found header status" << status.toString();
		if (status.toInt() != HttpStatusCode::OK)
		{
			qCCritical(update) << "Abort request, status " << status.toInt();
			mReply->abort();

			return;
		}
	}
	QVariant lastModified = mReply->header(QNetworkRequest::KnownHeaders::LastModifiedHeader);
	if (!lastModified.isNull())
	{
		qCDebug(update) << "Found header Last-Modified" << lastModified.toString();
		const QDateTime localIssueDate = mUpdateBackend->getIssueDate();
		if (localIssueDate.isValid() && lastModified.toDateTime() <= localIssueDate)
		{
			qCDebug(update) << "Abort request, Last-Modified is older than issue date of current data";
			mCancel = true;
			mReply->abort();

			return;
		}
	}
}


void UpdateService::onNetworkReplyFinished()
{
	mReply->deleteLater();

	if (mCancel)
	{
		Q_EMIT fireUpdateFinished();
		return;
	}

	const auto& cert = mReply->sslConfiguration().peerCertificate();
	if (cert.isNull() || !mTrustedUpdateCertificates.contains(cert))
	{
		qCCritical(update).nospace() << "Connection not secure [" << mNameForLog << "]";
		mUpdateBackend->processError(GlobalStatus::Code::Network_Ssl_Establishment_Error);
		Q_EMIT fireUpdateFinished();
		return;
	}

	if (mReply->error() != QNetworkReply::NoError)
	{
		qCCritical(update).nospace() << mReply->errorString() << " [" << mNameForLog << "]";
		mUpdateBackend->processError(NetworkManager::toStatus(mReply));
		Q_EMIT fireUpdateFinished();
		return;
	}

	mUpdateBackend->processSuccess(mReply->readAll());
	Q_EMIT fireUpdateFinished();
}


UpdateService::UpdateService(const QSharedPointer<UpdateBackend>& pUpdateBackend,
		const QString& pNameForLog)
	: mUpdateBackend(pUpdateBackend)
	, mNameForLog(pNameForLog)
	, mUpdateUrl()
	, mTrustedUpdateCertificates()
	, mReply()
	, mCancel(false)
{
	Q_ASSERT_X(pUpdateBackend, "UpdateService", "invalid update backend");
}


UpdateService::~UpdateService()
{
	if (mReply != nullptr)
	{
		if (mReply->isRunning())
		{
			qCDebug(update).nospace() << "Scheduling pending update request [" << mNameForLog << "] for deletion";
		}
		mReply->deleteLater();
	}
}


void UpdateService::setTrustedUpdateCertificates(const QVector<QSslCertificate>& pTrustedUpdateCertificates)
{
	mTrustedUpdateCertificates = pTrustedUpdateCertificates;
}


void UpdateService::setUpdateUrl(const QUrl& pUpdateUrl)
{
	mUpdateUrl = pUpdateUrl;
}


void UpdateService::runUpdate()
{
	qDebug(update) << QStringLiteral("Get %1 from update site:").arg(mNameForLog) << mUpdateUrl;
	QNetworkRequest request(mUpdateUrl);

	mCancel = false;
	mReply = NetworkManager::getGlobalInstance().get(request);

	connect(mReply.data(), &QNetworkReply::sslErrors, this, &UpdateService::onSslErrors);
	connect(mReply.data(), &QNetworkReply::encrypted, this, &UpdateService::onSslHandshakeDone);
	connect(mReply.data(), &QNetworkReply::metaDataChanged, this, &UpdateService::onMetadataChanged);
	connect(mReply.data(), &QNetworkReply::finished, this, &UpdateService::onNetworkReplyFinished);
}
