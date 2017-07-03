/*
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"
#include "NetworkManager.h"
#include "ReleaseNotesDownloader.h"

#include <QApplication>
#include <QSslCipher>

using namespace governikus;

ReleaseNotesDownloader::ReleaseNotesDownloader()
		: QObject(), mEventLoop(), mEncrypted(false), mReleaseNotes(tr("<h4>Download of release notes failed</h4>")), mReply()
{
}

ReleaseNotesDownloader::~ReleaseNotesDownloader()
{
}

void ReleaseNotesDownloader::checkCertificate(const QSslCertificate& pPeerCert)
{

	QSslConfiguration sslConfiguration = mReply->sslConfiguration();
	qDebug() << "Used session cipher" << sslConfiguration.sessionCipher();
	qDebug() << "Used session protocol:" << sslConfiguration.sessionProtocol();

    if (AppSettings::getInstance().getSecureStorage().getUpdateCertificates().contains(pPeerCert))
	{
		qDebug() << "Found trusted certificate for release notes:" << pPeerCert;
		mEncrypted = true;
	}
	else
	{
		qDebug() << "No trusted certificate found for release notes:" << pPeerCert;
		mReply->abort();
		mEventLoop.quit();
	}
}

void ReleaseNotesDownloader::encrypted()
{
	const QSslCertificate peerCert = mReply->sslConfiguration().peerCertificate();
	if (!peerCert.isNull())
	{
		checkCertificate(peerCert);
	}
}

void ReleaseNotesDownloader::finished()
{
	if (!mEncrypted)
	{
		checkCertificate(mReply->sslConfiguration().peerCertificate());
	}

	if (mEncrypted)
	{
		QVariant possibleRedirectUrl = mReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
		if (possibleRedirectUrl.toUrl().isEmpty())
		{
			mReleaseNotes = QString::fromUtf8(mReply->readAll());
			mEventLoop.quit();
		}
		else
		{
			qDebug() << "redirect: " << possibleRedirectUrl.toUrl();
			QNetworkRequest request(possibleRedirectUrl.toUrl());
			mReply.reset(NetworkManager::getGlobalInstance().get(request));
		}
	}
}

void ReleaseNotesDownloader::sslErrors(const QList<QSslError>& pErrors)
{
	for (const QSslError& error : pErrors)
	{
		mReleaseNotes.append(error.errorString());
		mReleaseNotes.append("<br/>");
		qCritical() << "Cannot check for release notes:" << error;
	}
}

QString ReleaseNotesDownloader::loadReleaseNotes(const QUrl& pUrl)
{
	qDebug() << "Download release notes:" << pUrl;
	QNetworkRequest request(pUrl);
	mReply.reset(NetworkManager::getGlobalInstance().get(request));

	connect(mReply.data(), &QNetworkReply::finished, this, &ReleaseNotesDownloader::finished);
	connect(mReply.data(), &QNetworkReply::sslErrors, this, &ReleaseNotesDownloader::sslErrors);
	connect(mReply.data(), &QNetworkReply::encrypted, this, &ReleaseNotesDownloader::encrypted);

	mEventLoop.exec();
	return mReleaseNotes;
}
