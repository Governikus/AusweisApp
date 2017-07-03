/*
 * \brief Class that ensures that all the icons and images specified
 *        in the application settings are in the application cache.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "IconService.h"
#include "ProviderSettings.h"

#include <QObject>
#include <QSet>
#include <QSharedPointer>
#include <QSslCertificate>


namespace governikus
{
class IconCache
	: public QObject
{
	Q_OBJECT

	private:
		QVector<QSslCertificate> mUpdateCertificates;

		QString mIconCacheFolderPath;

		QString mIconUpdateUrlBase;

		QStringList mRequiredIcons;

		QStringList mPendingIcons;

		QSharedPointer<IconService> mIconService;

		QString getIconPathInCache(const QString& pIcon) const;

		QStringList getIconsToDownload(const QStringList& pRequiredIcons) const;

		QUrl getRemoteIconUrl(const QString& pIcon) const;

		void processNextIcon();

		/*
		 * An icon can be mapped if the corresponding file or resource exists.
		 */
		bool canBeMapped(const QString& pIcon) const;

		QMap<QString, QString> makeIconMap(const QStringList& pIcons) const;

	private Q_SLOTS:
		void onProcessingFinished(const QString& pIcon);

	public:
		IconCache();

		~IconCache();

		void setTrustedUpdateCertificates(const QVector<QSslCertificate>& pUpdateCertificates);

		void setIconUpdateUrlBase(const QString& pIconUpdateUrlBase);

		QString getLocalIconUrl(const QString& pIcon) const;

		bool init();

	public Q_SLOTS:
		void onRequiredIcons(const QStringList& pIcons);

	Q_SIGNALS:
		void fireIconMapChanged(const QMap<QString, QString>& mIconMap);

};


} /* namespace governikus */
