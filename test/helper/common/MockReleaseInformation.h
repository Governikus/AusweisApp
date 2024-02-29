/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief MockReleaseInformation for Unittest
 */


#pragma once

#include "ReleaseInformation.h"

#include <QList>
#include <QTemporaryFile>

namespace governikus
{
class MockReleaseInformation
	: public ReleaseInformation
{
	Q_OBJECT

	private:
		bool mWasUpdated;
		const VersionNumber mVersion;
		int mCallcountUpdate;

		QSharedPointer<QTemporaryFile> mAnnounce;
		QSharedPointer<QTemporaryFile> mIssues;
		QList<QSharedPointer<QTemporaryFile>> mNotes;

		static QSharedPointer<QTemporaryFile> createDummyContent(const QStringList& pContent);

	public:
		MockReleaseInformation(const VersionNumber& pVersion, bool pConsiderOnlyThisVersion);

		void update() override;
		[[nodiscard]] bool requiresInitialUpdate() const override;
		[[nodiscard]] QStringList pathReleaseNotes() const override;
		[[nodiscard]] QString pathAnnouncements() const override;
		[[nodiscard]] QString pathIssues() const override;
		[[nodiscard]] VersionNumber versionNumber() const override;

		int callcountUpdate() const;

};
} // namespace governikus
