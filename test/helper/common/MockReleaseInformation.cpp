/*!
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */

#include "MockReleaseInformation.h"

#include <QTextStream>

using namespace governikus;

QSharedPointer<QTemporaryFile> MockReleaseInformation::createDummyContent(const QStringList& pContent)
{
	QSharedPointer<QTemporaryFile> tmpFile(new QTemporaryFile());
	if (tmpFile->open())
	{
		QTextStream ts(tmpFile.data());
		const auto newLine(QChar('\n'));
		for (const auto& line : pContent)
		{
			ts << line << newLine;
		}
	}

	return tmpFile;
}


MockReleaseInformation::MockReleaseInformation(const VersionNumber& pVersion, bool pConsiderOnlyThisVersion)
	: mWasUpdated(false)
{
	Q_UNUSED(pVersion)
	Q_UNUSED(pConsiderOnlyThisVersion)
}


void MockReleaseInformation::update()
{
	if (!mWasUpdated)
	{
		mAnnounce = createDummyContent({"Dummy announcements"});
		mIssues = createDummyContent({"Dummy issues"});

		mNotes << createDummyContent({"Dummy notes 0.1", "", "These are the updated notes."});
		mNotes << createDummyContent({"Dummy notes 0.0", "", "These are the initial notes."});

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
	return VersionNumber(QStringLiteral("1.2.3"));
}
