/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */

#include "MockReleaseInformation.h"

#include <QTextStream>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

QSharedPointer<QTemporaryFile> MockReleaseInformation::createDummyContent(const QStringList& pContent)
{
	QSharedPointer<QTemporaryFile> tmpFile(new QTemporaryFile());
	if (tmpFile->open())
	{
		QTextStream ts(tmpFile.data());
		const auto newLine('\n'_L1);
		for (const auto& line : pContent)
		{
			ts << line << newLine;
		}
	}

	return tmpFile;
}


MockReleaseInformation::MockReleaseInformation(const VersionNumber& pVersion, bool pConsiderOnlyThisVersion)
	: mWasUpdated(false)
	, mVersion(pVersion)
	, mCallcountUpdate(0)
{
	Q_UNUSED(pConsiderOnlyThisVersion)
}


void MockReleaseInformation::update()
{
	mCallcountUpdate++;
	if (!mWasUpdated)
	{
		mAnnounce = createDummyContent({"Dummy announcements"_L1});
		mIssues = createDummyContent({"Dummy issues"_L1});

		mNotes << createDummyContent({"Dummy notes 0.1"_L1, ""_L1, "These are the updated notes."_L1});
		mNotes << createDummyContent({"Dummy notes 0.0"_L1, ""_L1, "These are the initial notes."_L1});

		mWasUpdated = true;
		Q_EMIT fireInformationChanged();
	}
}


bool MockReleaseInformation::requiresInitialUpdate() const
{
	return !mWasUpdated;
}


QStringList MockReleaseInformation::pathReleaseNotes() const
{
	if (mNotes.size() == 0)
	{
		return QStringList();
	}

	QStringList notes;
	for (const auto& path : mNotes)
	{
		notes << path->fileName();
	}
	return notes;
}


QString MockReleaseInformation::pathAnnouncements() const
{

	return mAnnounce ? mAnnounce->fileName() : QString();
}


QString MockReleaseInformation::pathIssues() const
{
	return mIssues ? mIssues->fileName() : QString();
}


VersionNumber MockReleaseInformation::versionNumber() const
{
	return mVersion;
}


int MockReleaseInformation::callcountUpdate() const
{
	return mCallcountUpdate;
}
