/*!
 * \brief MockReleaseInformation for Unittest
 *
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */


#pragma once

#include "ReleaseInformation.h"

#include <QTemporaryFile>
#include <QVector>

namespace governikus
{
class MockReleaseInformation
	: public ReleaseInformation
{
	Q_OBJECT

	private:
		bool mWasUpdated;
		QSharedPointer<QTemporaryFile> mAnnounce;
		QSharedPointer<QTemporaryFile> mIssues;
		QVector<QSharedPointer<QTemporaryFile>> mNotes;

		static QSharedPointer<QTemporaryFile> createDummyContent(const QStringList& pContent);

	public:
		MockReleaseInformation(const VersionNumber& pVersion, bool pConsiderOnlyThisVersion);

		void update() override;
		[[nodiscard]] bool requiresInitialUpdate() const override;
		[[nodiscard]] QStringList pathReleaseNotes() const override;
		[[nodiscard]] QString pathAnnouncements() const override;
		[[nodiscard]] QString pathIssues() const override;
		[[nodiscard]] VersionNumber versionNumber() const override;
};
} // namespace governikus
