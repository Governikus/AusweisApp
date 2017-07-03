/*!
 * UpdateService.cpp
 *
 * \brief Generic implementation of a service providing an update mechanism for settings,
 *        images, and so on.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */


#include "NetworkManager.h"
#include "UpdateService.h"

#include <QLoggingCategory>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(update)


void UpdateService::onSslErrors(const QList<QSslError>& pErrors)
{
	for (const auto& error : pErrors)
	{
		qCWarning(update) << "(ignored)" << error.errorString();
	}
	mReply->ignoreSslErrors();
}


void UpdateService::onSslHandshakeDone()
{
	QSslConfiguration sslConfiguration = mReply->sslConfiguration();
	qCDebug(update) << "Used session cipher:" << sslConfiguration.sessionCipher();
	qCDebug(update) << "Used session protocol:" << sslConfiguration.sessionProtocol();

	QSslCertificate sslCert = sslConfiguration.peerCertificate();
	if (!mTrustedUpdateCertificates.contains(sslCert))
	{
		qCCritical(update).nospace() << "Untrusted certificate found [" << mNameForLog << "]: " << sslCert;
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
		if (status.toInt() != 200)
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
			mReply->abort();

			return;
		}
	}
}


void UpdateService::onNetworkReplyFinished()
{
	mReply->deleteLater();
	if (mReply->error() != QNetworkReply::NoError)
	{
		qCCritical(update).nospace() << mReply->errorString() << " [" << mNameForLog << "]";
		mUpdateBackend->processError();
		Q_EMIT fireUpdateFinished();

		return;
	}
	else if (mReply->sslConfiguration().peerCertificate().isNull())
	{
		qCCritical(update).nospace() << "Connection not encrypted [" << mNameForLog << "]";
		mUpdateBackend->processError();
		Q_EMIT fireUpdateFinished();

		return;
	}

	mUpdateBackend->processSuccess(mReply->readAll());

	Q_EMIT fireUpdateFinished();
}


QSharedPointer<UpdateBackend> UpdateService::getUpdateBackend()
{
	return mUpdateBackend;
}


UpdateService::UpdateService(const QSharedPointer<UpdateBackend>& pUpdateBackend,
		const QString& pNameForLog)
	: mUpdateBackend(pUpdateBackend)
	, mNameForLog(pNameForLog)
	, mUpdateUrl()
	, mTrustedUpdateCertificates()
	, mReply()
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
	qCDebug(update) << QStringLiteral("Get %1 from update site %2").arg(mNameForLog, mUpdateUrl.toString());
	QNetworkRequest request(mUpdateUrl);

	mReply = NetworkManager::getGlobalInstance().get(request);

	connect(mReply.data(), &QNetworkReply::sslErrors, this, &UpdateService::onSslErrors);
	connect(mReply.data(), &QNetworkReply::encrypted, this, &UpdateService::onSslHandshakeDone);
	connect(mReply.data(), &QNetworkReply::metaDataChanged, this, &UpdateService::onMetadataChanged);
	connect(mReply.data(), &QNetworkReply::finished, this, &UpdateService::onNetworkReplyFinished);
}
