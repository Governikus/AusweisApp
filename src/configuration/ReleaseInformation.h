/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Provides information about the App release
 */

#pragma once

#include "UpdatableFile.h"
#include "VersionNumber.h"

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>

namespace governikus
{

class ReleaseInformation
	: public QObject
{
	Q_OBJECT

	private:
		const QList<QSharedPointer<UpdatableFile>> mReleaseNotes;
		const QSharedPointer<UpdatableFile> mAnnouncements;
		const QSharedPointer<UpdatableFile> mIssues;
		const VersionNumber mVersion;

		static QSharedPointer<UpdatableFile> getFile(const QString& pFile);
		static QList<QSharedPointer<UpdatableFile>> getReleaseNotes(const VersionNumber& pVersion, bool pConsiderOnlyThisVersion);

	Q_SIGNALS:
		void fireInformationChanged();

	public:
		ReleaseInformation();
		explicit ReleaseInformation(const VersionNumber& pVersion, bool pConsiderOnlyThisVersion = false);
		~ReleaseInformation() override = default;

		virtual void update();
		[[nodiscard]] virtual bool requiresInitialUpdate() const;
		[[nodiscard]] virtual QStringList pathReleaseNotes() const;
		[[nodiscard]] virtual QString pathAnnouncements() const;
		[[nodiscard]] virtual QString pathIssues() const;
		[[nodiscard]] virtual VersionNumber versionNumber() const;
};

} // namespace governikus
