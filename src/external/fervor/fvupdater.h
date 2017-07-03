#ifndef FVUPDATER_H
#define FVUPDATER_H

#include <QObject>
#include <QMutex>
#include <QNetworkReply>
#include <QUrl>
#include <QXmlStreamReader>
class FvUpdateWindow;
class FvAvailableUpdate;


class FvUpdater : public QObject
{
	Q_OBJECT

public:
	static bool DisableSilentCheck;

	// Singleton
	static FvUpdater* sharedUpdater();
	static void drop();

	void SetFeedURL(const QUrl &feedURL);

public Q_SLOTS:

	// Check for updates
	bool CheckForUpdates(bool silentAsMuchAsItCouldGet = true);

	// Aliases
	bool CheckForUpdatesSilent();
	bool CheckForUpdatesNotSilent();


	//
	// ---------------------------------------------------
	// ---------------------------------------------------
	// ---------------------------------------------------
	// ---------------------------------------------------
	//

protected:

  enum msgType {
    INFO_MESSAGE,         // shown always for users
    NO_UPDATE_MESSAGE,    // shown message only in not-silent mode, but modified
    CRITICAL_MESSAGE      // shown always
  };

	friend class FvUpdateWindow;		// Uses GetProposedUpdate() and others
	friend class FvUpdateConfirmDialog;	// Uses GetProposedUpdate() and others
	FvAvailableUpdate* GetProposedUpdate();


protected Q_SLOTS:

	// Update window button Q_SLOTS
	void InstallUpdate();
	void SkipUpdate();
	void RemindMeLater();

private:

	//
	// Singleton business
	//
	// (we leave just the declarations, so the compiler will warn us if we try
	//  to use those two functions by accident)
	FvUpdater();							// Hide main constructor
	~FvUpdater();							// Hide main destructor
	Q_DISABLE_COPY(FvUpdater)


	static FvUpdater* m_Instance;			// Singleton instance


	//
	// Windows / dialogs
	//
	FvUpdateWindow* m_updaterWindow;								// Updater window (NULL if not shown)
	void showUpdaterWindowUpdatedWithCurrentUpdateProposal();		// Show updater window
	void hideUpdaterWindow();										// Hide + destroy m_updaterWindow
	void updaterWindowWasClosed();									// Sent by the updater window when it gets closed

	// Available update (NULL if not fetched)
	FvAvailableUpdate* m_proposedUpdate;

	// If true, don't show the error dialogs and the "no updates." dialog
	// (silentAsMuchAsItCouldGet from CheckForUpdates() goes here)
	// Useful for automatic update checking upon application startup.
	bool m_silentAsMuchAsItCouldGet;

	// Dialogs (notifications)
	void showErrorDialog(QString message, msgType type = NO_UPDATE_MESSAGE);			// Show an error message
	void showInformationDialog(const QString& message, bool showEvenInSilentMode = false);		// Show an informational message


	//
	// HTTP feed fetcher infrastructure
	//
	QUrl m_feedURL;					// Feed URL that will be fetched
	QNetworkReply* m_reply;
	bool m_encrypted;
	bool m_httpRequestAborted;
	bool m_securityError;

	void startDownloadFeed(const QUrl& url);	// Start downloading feed
	void cancelDownloadFeed();			// Stop downloading the current feed
	void httpFeedSslCheckPeerCert(const QSslCertificate& pPeerCert);

private Q_SLOTS:

	void httpFeedReadyRead();
	void httpFeedUpdateDataReadProgress(qint64 bytesRead,
										qint64 totalBytes);
	void httpFeedDownloadFinished();
	void httpFeedSslErrors(const QList<QSslError>& pErrors);
	void httpFeedSslEncrypted();


private:

	//
	// XML parser
	//
	QXmlStreamReader m_xml;				// XML data collector and parser
	bool xmlParseFeed();				// Parse feed in m_xml
	bool searchDownloadedFeedForUpdates(const QString& xmlTitle,
										QString xmlLink,
										QString xmlReleaseNotesLink,
										const QString& xmlPubDate,
										const QString& xmlEnclosureUrl,
										const QString& xmlEnclosureVersion,
										const QString& xmlEnclosurePlatform,
										unsigned long xmlEnclosureLength,
										const QString& xmlEnclosureType);


	//
	// Helpers
	//
	void installTranslator();			// Initialize translation mechanism

};

#endif // FVUPDATER_H
