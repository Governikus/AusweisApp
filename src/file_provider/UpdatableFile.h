/*!
 * \brief Class that provides access to a file identified by a section and a name.
 * The file is looked up in the local cache or in the application's resource bundle.
 * This class also handles the synchronization of the local cache with the content
 * provided by a server.
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"

#include <QObject>
#include <QUrl>

#include <functional>

class test_UpdatableFile;
class test_ReaderConfiguration;


namespace governikus
{
class UpdatableFile
	: public QObject
{
	Q_OBJECT

	private:
		friend class ::test_UpdatableFile;
		friend class ::test_ReaderConfiguration;

		const QString mSection;
		const QString mName;
		QString mDefaultPath;
		const QString mSectionCachePath;
		const QUrl mUpdateUrl;
		bool mUpdateRunning;

		const QString& getName();
		QDateTime cacheTimestamp() const;
		const QString& getSectionCachePath() const;

		QString qrcPath() const;
		QString cachePath() const;
		QUrl updateUrl(const QString& pSection, const QString& pName);
		QString dirtyFilePath() const;
		QString sectionCachePath(const QString& pSection) const;
		QString makeSectionCachePath(const QString& pSection);
		void cleanupAfterUpdate(const std::function<void()>& pCustomAction);
		bool writeDataToFile(const QByteArray& pData, const QString& pFilePath, bool pOverwrite = false);

	private Q_SLOTS:
		void onDownloadSuccess(const QUrl& pUpdateUrl, const QDateTime& pNewTimestamp, const QByteArray& pData);
		void onDownloadFailed(const QUrl& pUpdateUrl, GlobalStatus::Code pErrorCode);
		void onDownloadUnnecessary(const QUrl& pUpdateUrl);

	public:
		UpdatableFile(const QString& pSection, const QString& pName, const QString& pDefaultPath = QString());
		virtual ~UpdatableFile() = default;

		QUrl lookupUrl();
		QString lookupPath();
		void setDefaultPath(const QString& pPath);
		const QString& getDefaultPath() const;

		void update();
		bool isDirty() const;
		void clearDirty() const;
		void markDirty() const;

	Q_SIGNALS:
		void fireUpdated();
		void fireNoUpdateAvailable();

};

} // namespace governikus
