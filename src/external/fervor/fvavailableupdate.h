#ifndef FVAVAILABLEUPDATE_H
#define FVAVAILABLEUPDATE_H

#include <QObject>
#include <QUrl>

class FvAvailableUpdate : public QObject
{
	Q_OBJECT
public:
	explicit FvAvailableUpdate(QObject *pParent = nullptr);

	QString GetTitle();
	void SetTitle(const QString& title);

	QUrl GetReleaseNotesLink();
	void SetReleaseNotesLink(const QUrl& releaseNotesLink);
	void SetReleaseNotesLink(const QString& releaseNotesLink);

	QString GetReleaseNotes();
	void SetReleaseNotes(const QString& releaseNotes);

	QString GetPubDate();
	void SetPubDate(const QString& pubDate);

	QUrl GetEnclosureUrl();
	void SetEnclosureUrl(const QUrl& enclosureUrl);
	void SetEnclosureUrl(const QString& enclosureUrl);

	QString GetEnclosureVersion();
	void SetEnclosureVersion(const QString& enclosureVersion);

	QString GetEnclosurePlatform();
	void SetEnclosurePlatform(const QString& enclosurePlatform);

	unsigned long GetEnclosureLength();
	void SetEnclosureLength(unsigned long enclosureLength);

	QString GetEnclosureType();
	void SetEnclosureType(const QString& enclosureType);

private:
	QString m_title;
	QUrl m_releaseNotesLink;
	QString m_releaseNotes;
	QString m_pubDate;
	QUrl m_enclosureUrl;
	QString m_enclosureVersion;
	QString m_enclosurePlatform;
	unsigned long m_enclosureLength;
	QString m_enclosureType;

};

#endif // FVAVAILABLEUPDATE_H
