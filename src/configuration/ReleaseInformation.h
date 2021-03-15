/*!
 * \brief Provides information about the App release
 *
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UpdatableFile.h"
#include "VersionNumber.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QVector>

namespace governikus
{

class ReleaseInformation
	: public QObject
{
	Q_OBJECT

	private:
		const QVector<QSharedPointer<UpdatableFile>> mReleaseNotes;
		const QSharedPointer<UpdatableFile> mAnnouncements;
		const QSharedPointer<UpdatableFile> mIssues;
		const VersionNumber mVersion;

		static QSharedPointer<UpdatableFile> getFile(const QString& pFile);
		static QVector<QSharedPointer<UpdatableFile>> getReleaseNotes(const VersionNumber& pVersion, bool pConsiderOnlyThisVersion);

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
