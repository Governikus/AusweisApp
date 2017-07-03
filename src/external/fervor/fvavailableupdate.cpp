#include "fvavailableupdate.h"

FvAvailableUpdate::FvAvailableUpdate(QObject *pParent)
	: QObject(pParent)
	, m_title()
	, m_releaseNotesLink()
	, m_releaseNotes()
	, m_pubDate()
	, m_enclosureUrl()
	, m_enclosureVersion()
	, m_enclosurePlatform()
	, m_enclosureLength(0)
	, m_enclosureType()
{
	// noop
}

QString FvAvailableUpdate::GetTitle()
{
	return m_title;
}

void FvAvailableUpdate::SetTitle(const QString& title)
{
	m_title = title;
}

QUrl FvAvailableUpdate::GetReleaseNotesLink()
{
	return m_releaseNotesLink;
}

void FvAvailableUpdate::SetReleaseNotesLink(const QUrl& releaseNotesLink)
{
	m_releaseNotesLink = releaseNotesLink;
}

void FvAvailableUpdate::SetReleaseNotesLink(const QString& releaseNotesLink)
{
	SetReleaseNotesLink(QUrl(releaseNotesLink));
}

QString FvAvailableUpdate::GetReleaseNotes()
{
	return m_releaseNotes;
}

void FvAvailableUpdate::SetReleaseNotes(const QString &releaseNotes)
{
	m_releaseNotes = releaseNotes;
}

QString FvAvailableUpdate::GetPubDate()
{
	return m_pubDate;
}

void FvAvailableUpdate::SetPubDate(const QString& pubDate)
{
	m_pubDate = pubDate;
}

QUrl FvAvailableUpdate::GetEnclosureUrl()
{
	return m_enclosureUrl;
}

void FvAvailableUpdate::SetEnclosureUrl(const QUrl& enclosureUrl)
{
	m_enclosureUrl = enclosureUrl;
}

void FvAvailableUpdate::SetEnclosureUrl(const QString& enclosureUrl)
{
	SetEnclosureUrl(QUrl(enclosureUrl));
}

QString FvAvailableUpdate::GetEnclosureVersion()
{
	return m_enclosureVersion;
}

void FvAvailableUpdate::SetEnclosureVersion(const QString& enclosureVersion)
{
	m_enclosureVersion = enclosureVersion;
}

QString FvAvailableUpdate::GetEnclosurePlatform()
{
	return m_enclosurePlatform;
}

void FvAvailableUpdate::SetEnclosurePlatform(const QString& enclosurePlatform)
{
	m_enclosurePlatform = enclosurePlatform;
}

unsigned long FvAvailableUpdate::GetEnclosureLength()
{
	return m_enclosureLength;
}

void FvAvailableUpdate::SetEnclosureLength(unsigned long enclosureLength)
{
	m_enclosureLength = enclosureLength;
}

QString FvAvailableUpdate::GetEnclosureType()
{
	return m_enclosureType;
}

void FvAvailableUpdate::SetEnclosureType(const QString& enclosureType)
{
	m_enclosureType = enclosureType;
}
