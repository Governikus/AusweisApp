/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Class that provides access to a file identified by a section and a name.
 * The file is looked up in the local cache or in the application's resource bundle.
 * This class also handles the synchronization of the local cache with the content
 * provided by a server.
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
	friend class ::test_UpdatableFile;
	friend class ::test_ReaderConfiguration;

	private:
		const QString mSection;
		const QString mName;
		QString mDefaultPath;
		const QString mSectionCachePath;
		const QUrl mUpdateUrl;
		bool mUpdateRunning;

		[[nodiscard]] const QString& getName() const;
		[[nodiscard]] QDateTime cacheTimestamp() const;
		[[nodiscard]] const QString& getSectionCachePath() const;

		[[nodiscard]] QString qrcPath() const;
		[[nodiscard]] QString cachePath() const;
		[[nodiscard]] QUrl updateUrl(const QString& pSection, const QString& pName) const;
		[[nodiscard]] QString dirtyFilePath() const;
		[[nodiscard]] QString sectionCachePath(const QString& pSection) const;
		[[nodiscard]] QString makeSectionCachePath(const QString& pSection) const;
		void cleanupAfterUpdate(const std::function<void()>& pCustomAction);
		bool writeDataToFile(const QByteArray& pData, const QString& pFilePath) const;

	private Q_SLOTS:
		void onDownloadSuccess(const QUrl& pUpdateUrl, const QDateTime& pNewTimestamp, const QByteArray& pData);
		void onDownloadFailed(const QUrl& pUpdateUrl, GlobalStatus::Code pErrorCode);
		void onDownloadUnnecessary(const QUrl& pUpdateUrl);

	public:
		UpdatableFile(const QString& pSection, const QString& pName, const QString& pDefaultPath = QString());
		~UpdatableFile() override = default;

		QUrl lookupUrl();
		QString lookupPath();
		bool forEachLookupPath(const std::function<bool(const QString&)>& pValidate);

		void setDefaultPath(const QString& pPath);
		[[nodiscard]] const QString& getDefaultPath() const;

		void update();
		[[nodiscard]] bool isDirty() const;
		void clearDirty() const;
		void markDirty() const;

	Q_SIGNALS:
		void fireUpdated();
		void fireNoUpdateAvailable();

};

} // namespace governikus
