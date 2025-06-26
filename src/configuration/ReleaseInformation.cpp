/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
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
		if (mVersion.isBetaVersion())
		{
			connect(releaseNotesFile.data(), &UpdatableFile::fireNoUpdateAvailable, this, &ReleaseInformation::fireInformationChanged);
		}
	}

	connect(mAnnouncements.data(), &UpdatableFile::fireUpdated, this, &ReleaseInformation::fireInformationChanged);
	connect(mIssues.data(), &UpdatableFile::fireUpdated, this, &ReleaseInformation::fireInformationChanged);
	if (mVersion.isBetaVersion())
	{
		connect(mAnnouncements.data(), &UpdatableFile::fireNoUpdateAvailable, this, &ReleaseInformation::fireInformationChanged);
		connect(mIssues.data(), &UpdatableFile::fireNoUpdateAvailable, this, &ReleaseInformation::fireInformationChanged);
	}
}


QSharedPointer<UpdatableFile> ReleaseInformation::getFile(const QString& pFile)
{
	const auto locale = LanguageLoader::getLocaleCode();
	return Env::getSingleton<FileProvider>()->getFile(QStringLiteral("releasenotes/%1").arg(locale), pFile);
}


QList<QSharedPointer<UpdatableFile>> ReleaseInformation::getReleaseNotes(const VersionNumber& pVersion, bool pConsiderOnlyThisVersion)
{
	const auto versionNumber = pVersion.getVersionNumber();

	if (versionNumber.microVersion() >= 100)
	{
		const auto betaVersion = QVersionNumber(versionNumber.majorVersion(), versionNumber.minorVersion(), 100);
		return {getFile(QStringLiteral("%1.txt").arg(betaVersion.toString()))};
	}

	if (pConsiderOnlyThisVersion)
	{
		return {getFile(QStringLiteral("%1.txt").arg(versionNumber.toString()))};
	}

	QList<QSharedPointer<UpdatableFile>> releaseNotes;
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
	if (mVersion.isBetaVersion())
	{
		return true;
	}

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
