/*!
 * UpdateService.h
 *
 * \brief Generic implementation of a service providing an update mechanism for settings,
 *        application icons, and so on.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */


#pragma once


#include "UpdateBackend.h"

#include <QNetworkReply>
#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include <QSslCipher>
#include <QUrl>


namespace governikus
{
class UpdateService
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<UpdateBackend> mUpdateBackend;

		const QString mNameForLog;

		QUrl mUpdateUrl;

		QVector<QSslCertificate> mTrustedUpdateCertificates;

		QPointer<QNetworkReply> mReply;
		bool mCancel;

	private Q_SLOTS:
		void onSslErrors(const QList<QSslError>& pErrors);

		void onSslHandshakeDone();

		void onMetadataChanged();

		void onNetworkReplyFinished();

	protected:
		template<typename T = UpdateBackend>
		QSharedPointer<T> getUpdateBackend()
		{
			return mUpdateBackend.staticCast<T>();
		}


		template<typename T = UpdateBackend>
		QSharedPointer<const T> getUpdateBackend() const
		{
			return mUpdateBackend.staticCast<const T>();
		}


	public:
		UpdateService(const QSharedPointer<UpdateBackend>& pUpdateBackend,
				const QString& pNameForLog);

		virtual ~UpdateService();

		void setUpdateUrl(const QUrl& pUpdateUrl);
		virtual void setTrustedUpdateCertificates(const QVector<QSslCertificate>& pTrustedUpdateCertificates);
		void runUpdate();

	Q_SIGNALS:
		void fireUpdateFinished();

};

} /* namespace governikus */
