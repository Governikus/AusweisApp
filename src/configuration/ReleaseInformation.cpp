/*!
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ReleaseInformation.h"

#include "FileProvider.h"
#include "LanguageLoader.h"
#include "VersionNumber.h"

using namespace governikus;

ReleaseInformation::ReleaseInformation()
	: ReleaseInformation(VersionNumber::getApplicationVersion())
{
}


ReleaseInformation::ReleaseInformation(const VersionNumber& pVersion, bool pConsiderOnlyThisVersion)
	: QObject()
	, mReleaseNotes(getReleaseNotes(pVersion, pConsiderOnlyThisVersion))
	, mAnnouncements(getFile(QStringLiteral("announce.txt")))
	, mIssues(getFile(QStringLiteral("issues.txt")))
	, mVersion(pVersion)
{
	for (const auto& releaseNotesFile : mReleaseNotes)
	{
		connect(releaseNotesFile.data(), &UpdatableFile::fireUpdated, this, &ReleaseInformation::fireInformationChanged);
	}

	connect(mAnnouncements.data(), &UpdatableFile::fireUpdated, this, &ReleaseInformation::fireInformationChanged);
	connect(mIssues.data(), &UpdatableFile::fireUpdated, this, &ReleaseInformation::fireInformationChanged);
}


QSharedPointer<UpdatableFile> ReleaseInformation::getFile(const QString& pFile)
{
	const auto locale = LanguageLoader::getLocaleCode();
	return Env::getSingleton<FileProvider>()->getFile(QStringLiteral("releasenotes/%1").arg(locale), pFile);
}


QVector<QSharedPointer<UpdatableFile>> ReleaseInformation::getReleaseNotes(const VersionNumber& pVersion, bool pConsiderOnlyThisVersion)
{
	const auto versionNumber = pVersion.getVersionNumber();

	if (pConsiderOnlyThisVersion)
	{
		return {getFile(QStringLiteral("%1.txt").arg(versionNumber.toString()))};
	}

	QVector<QSharedPointer<UpdatableFile>> releaseNotes;

	for (int i = versionNumber.microVersion(); i >= 0; --i)
	{
		const auto previousVersion = QVersionNumber(versionNumber.majorVersion(), versionNumber.minorVersion(), i);
		releaseNotes << getFile(QStringLiteral("%1.txt").arg(previousVersion.toString()));
	}

	return releaseNotes;
}


void ReleaseInformation::update()
{
	for (const auto& releaseNotesFile : mReleaseNotes)
	{
		releaseNotesFile->update();
	}

	mAnnouncements->update();
	mIssues->update();
}


bool ReleaseInformation::requiresInitialUpdate() const
{
	for (const auto& releaseNotesFile : mReleaseNotes)
	{
		if (releaseNotesFile->lookupPath().isEmpty())
		{
			return true;
		}
	}

	return mAnnouncements->lookupPath().isEmpty() || mIssues->lookupPath().isEmpty();
}


QStringList ReleaseInformation::pathReleaseNotes() const
{
	QStringList paths;
	for (const auto& releaseNotesFile : mReleaseNotes)
	{
		paths << releaseNotesFile->lookupPath();
	}
	return paths;
}


QString ReleaseInformation::pathAnnouncements() const
{
	return mAnnouncements->lookupPath();
}


QString ReleaseInformation::pathIssues() const
{
	return mIssues->lookupPath();
}


VersionNumber ReleaseInformation::versionNumber() const
{
	return mVersion;
}
