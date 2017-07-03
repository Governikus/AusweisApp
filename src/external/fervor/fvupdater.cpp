#include "fvupdater.h"
#include "fvupdatewindow.h"
#include "fvplatform.h"
#include "fvignoredversions.h"
#include "fvavailableupdate.h"

#include "AppSettings.h"
#include "NetworkManager.h"
#include "ReleaseNotesDownloader.h"

#include <QApplication>
#include <QtNetwork>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDebug>
#include <QIcon>


#ifdef FV_DEBUG
	// Unit tests
#	include "fvversioncomparatortest.h"
#endif

using governikus::NetworkManager;
using governikus::AppSettings;

FvUpdater* FvUpdater::m_Instance = nullptr;
bool FvUpdater::DisableSilentCheck = false;

FvUpdater* FvUpdater::sharedUpdater()
{
	static QMutex mutex;
	if (! m_Instance) {
		mutex.lock();

		if (! m_Instance) {
			m_Instance = new FvUpdater;
		}

		mutex.unlock();
	}

	return m_Instance;
}

void FvUpdater::drop()
{
	static QMutex mutex;
	mutex.lock();
	delete m_Instance;
	m_Instance = nullptr;
	mutex.unlock();
}

FvUpdater::FvUpdater()
	: QObject(nullptr)
	, m_updaterWindow(nullptr)
	, m_proposedUpdate(nullptr)
	, m_silentAsMuchAsItCouldGet(false)
	, m_feedURL()
	, m_reply(nullptr)
	, m_encrypted(false)
	, m_httpRequestAborted(false)
	, m_securityError(false)
	, m_xml()
{
#ifdef FV_DEBUG
	// Unit tests
	FvVersionComparatorTest* test = new FvVersionComparatorTest();
	test->runAll();
	delete test;
#endif

}

FvUpdater::~FvUpdater()
{
	if (m_proposedUpdate) {
		delete m_proposedUpdate;
		m_proposedUpdate = nullptr;
	}

	hideUpdaterWindow();
}

void FvUpdater::showUpdaterWindowUpdatedWithCurrentUpdateProposal()
{
	// Destroy window if already exists
	hideUpdaterWindow();

	// Create a new window
	m_updaterWindow = new FvUpdateWindow();
	m_updaterWindow->UpdateWindowWithCurrentProposedUpdate();
	m_updaterWindow->show();
}

void FvUpdater::hideUpdaterWindow()
{
	if (m_updaterWindow) {
		if (! m_updaterWindow->close()) {
			qWarning() << "Update window didn't close, leaking memory from now on";
		}

		// not deleting because of Qt::WA_DeleteOnClose

		m_updaterWindow = nullptr;
	}
}

void FvUpdater::updaterWindowWasClosed()
{
	// (Re-)nullify a pointer to a destroyed QWidget or you're going to have a bad time.
	m_updaterWindow = nullptr;
}


void FvUpdater::SetFeedURL(const QUrl& feedURL)
{
	m_feedURL = feedURL;
}


FvAvailableUpdate* FvUpdater::GetProposedUpdate()
{
	return m_proposedUpdate;
}


void FvUpdater::InstallUpdate()
{
	qDebug() << "Install update";

	FvAvailableUpdate* proposedUpdate = GetProposedUpdate();
	if (! proposedUpdate) {
		qWarning() << "Proposed update is NULL (shouldn't be at this point)";
		return;
	}

	// Open a link
	if (! QDesktopServices::openUrl(proposedUpdate->GetEnclosureUrl())) {
		showErrorDialog(tr("Unable to open this link in a browser. Please copy and paste the link into the address bar of your browser."), CRITICAL_MESSAGE);
		return;
	}

	hideUpdaterWindow();
}

void FvUpdater::SkipUpdate()
{
	qDebug() << "Skip update";

	FvAvailableUpdate* proposedUpdate = GetProposedUpdate();
	if (! proposedUpdate) {
		qWarning() << "Proposed update is NULL (shouldn't be at this point)";
		return;
	}

	// Start ignoring this particular version
	FVIgnoredVersions::IgnoreVersion(proposedUpdate->GetEnclosureVersion());

	hideUpdaterWindow();
}

void FvUpdater::RemindMeLater()
{
	qDebug() << "Remind me later";

	hideUpdaterWindow();
}


bool FvUpdater::CheckForUpdates(bool silentAsMuchAsItCouldGet)
{
	if (m_feedURL.isEmpty()) {
		qCritical() << "Please set feed URL via setFeedURL() before calling CheckForUpdates().";
		return false;
	}

	m_silentAsMuchAsItCouldGet = silentAsMuchAsItCouldGet;

	// Check if application's organization name and domain are set, fail otherwise
	// (nowhere to store QSettings to)
	if (QApplication::organizationName().isEmpty()) {
		qCritical() << "QApplication::organizationName is not set. Please do that.";
		return false;
	}
	if (QApplication::organizationDomain().isEmpty()) {
		qCritical() << "QApplication::organizationDomain is not set. Please do that.";
		return false;
	}

	cancelDownloadFeed();
	m_httpRequestAborted = false;
	startDownloadFeed(m_feedURL);

	return true;
}

bool FvUpdater::CheckForUpdatesSilent()
{
	if (!DisableSilentCheck)
		return CheckForUpdates(true);

	return false;
}

bool FvUpdater::CheckForUpdatesNotSilent()
{
	return CheckForUpdates(false);
}


void FvUpdater::startDownloadFeed(const QUrl& url)
{
	m_xml.clear();

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml");

	m_reply = governikus::NetworkManager::getGlobalInstance().get(request);
	connect(m_reply, &QNetworkReply::readyRead, this, &FvUpdater::httpFeedReadyRead);
	connect(m_reply, &QNetworkReply::downloadProgress, this, &FvUpdater::httpFeedUpdateDataReadProgress);
	connect(m_reply, &QNetworkReply::finished, this, &FvUpdater::httpFeedDownloadFinished);
	connect(m_reply, &QNetworkReply::sslErrors, this, &FvUpdater::httpFeedSslErrors);
	connect(m_reply, &QNetworkReply::encrypted, this, &FvUpdater::httpFeedSslEncrypted);
}

void FvUpdater::httpFeedSslErrors(const QList<QSslError>& pErrors)
{
	for (const QSslError& error : pErrors)
	{
		qWarning() << "(ignored)" << error.errorString();
	}
	m_reply->ignoreSslErrors();
}

void FvUpdater::cancelDownloadFeed()
{
	if (m_reply) {
		m_httpRequestAborted = true;
		m_reply->abort();
	}
}

void FvUpdater::httpFeedSslCheckPeerCert(const QSslCertificate& pPeerCert)
{
    if (AppSettings::getInstance().getSecureStorage().getUpdateCertificates().contains(pPeerCert))
	{
		qDebug() << "Found trusted update certificate:" << pPeerCert;
		m_encrypted = true;
	}
	else
	{
		qDebug() << "No trusted update certificate found:" << pPeerCert;
		m_securityError = true;
		m_reply->abort();
	}
}

void FvUpdater::httpFeedSslEncrypted()
{
	QSslConfiguration sslConfiguration = m_reply->sslConfiguration();
	qDebug() << "Used session cipher" << sslConfiguration.sessionCipher();
	qDebug() << "Used session protocol:" << sslConfiguration.sessionProtocol();

	const QSslCertificate peerCert = m_reply->sslConfiguration().peerCertificate();
	if (!peerCert.isNull())
	{
		httpFeedSslCheckPeerCert(peerCert);
	}
}

void FvUpdater::httpFeedReadyRead()
{
	if (!m_encrypted)
	{
		const QSslCertificate peerCert = m_reply->sslConfiguration().peerCertificate();
		httpFeedSslCheckPeerCert(peerCert);
	}

	// this slot gets called every time the QNetworkReply has new data.
	// We read all of its new data and write it into the file.
	// That way we use less RAM than when reading it at the finished()
	// signal of the QNetworkReply
	if (m_encrypted)
	{
		m_xml.addData(m_reply->readAll());
	}
}

void FvUpdater::httpFeedUpdateDataReadProgress(qint64 bytesRead,
											   qint64 totalBytes)
{
	Q_UNUSED(bytesRead);
	Q_UNUSED(totalBytes);

	if (m_httpRequestAborted) {
		return;
	}
}

void FvUpdater::httpFeedDownloadFinished()
{
	if (m_httpRequestAborted) {
		m_reply->deleteLater();
		return;
	}

	QVariant redirectionTarget = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (m_reply->error()) {
		// Error.
		showErrorDialog(tr("An error occurred. Please contact our support at <a href=\"https://www.ausweisapp.bund.de/en/service/support/\">AusweisApp2 Support</a>."), CRITICAL_MESSAGE);

	} else if (! redirectionTarget.isNull()) {
		QUrl newUrl = m_feedURL.resolved(redirectionTarget.toUrl());

		m_feedURL = newUrl;
		m_reply->deleteLater();

		startDownloadFeed(m_feedURL);
		return;

	} else {

		// Done.
		xmlParseFeed();

	}

	m_reply->deleteLater();
	m_reply = nullptr;
}

bool FvUpdater::xmlParseFeed()
{
	QString currentTag, currentQualifiedTag;

	QString xmlTitle, xmlLink, xmlReleaseNotesLink, xmlPubDate, xmlEnclosureUrl,
			xmlEnclosureVersion, xmlEnclosurePlatform, xmlEnclosureType;
	unsigned long xmlEnclosureLength = 0;

	// Parse
	while (! m_xml.atEnd()) {

		m_xml.readNext();

		if (m_xml.isStartElement()) {

			currentTag = m_xml.name().toString();
			currentQualifiedTag = m_xml.qualifiedName().toString();

			if (m_xml.name() == "item") {

				xmlTitle.clear();
				xmlLink.clear();
				xmlReleaseNotesLink.clear();
				xmlPubDate.clear();
				xmlEnclosureUrl.clear();
				xmlEnclosureVersion.clear();
				xmlEnclosurePlatform.clear();
				xmlEnclosureLength = 0;
				xmlEnclosureType.clear();

			} else if (m_xml.name() == "enclosure") {

				QXmlStreamAttributes attribs = m_xml.attributes();

				if (attribs.hasAttribute(QStringLiteral("fervor:platform"))) {

					if (FvPlatform::CurrentlyRunningOnPlatform(attribs.value(QStringLiteral("fervor:platform")).toString().trimmed())) {

						xmlEnclosurePlatform = attribs.value(QStringLiteral("fervor:platform")).toString().trimmed();

						if (attribs.hasAttribute(QStringLiteral("url"))) {
							xmlEnclosureUrl = attribs.value(QStringLiteral("url")).toString().trimmed();
						} else {
							xmlEnclosureUrl = QLatin1String("");
						}

                        // First check for Sparkle's version, then overwrite with Fervor's version (if any)
                        if (attribs.hasAttribute(QStringLiteral("sparkle:version"))) {
                            QString candidateVersion = attribs.value(QStringLiteral("sparkle:version")).toString().trimmed();
                            if (! candidateVersion.isEmpty()) {
                                xmlEnclosureVersion = candidateVersion;
                            }
                        }
                        if (attribs.hasAttribute(QStringLiteral("fervor:version"))) {
                            QString candidateVersion = attribs.value(QStringLiteral("fervor:version")).toString().trimmed();
                            if (! candidateVersion.isEmpty()) {
                                xmlEnclosureVersion = candidateVersion;
                            }
                        }

						if (attribs.hasAttribute(QStringLiteral("length"))) {
							xmlEnclosureLength = attribs.value(QStringLiteral("length")).toString().toLong();
						} else {
							xmlEnclosureLength = 0;
						}
						if (attribs.hasAttribute(QStringLiteral("type"))) {
							xmlEnclosureType = attribs.value(QStringLiteral("type")).toString().trimmed();
						} else {
							xmlEnclosureType = QLatin1String("");
						}

					}

				}

			}

		} else if (m_xml.isEndElement()) {

			if (m_xml.name() == "item" && !xmlEnclosurePlatform.isEmpty()) {

				// That's it - we have analyzed a single <item> and we'll stop
				// here (because the topmost is the most recent one, and thus
				// the newest version.

				return searchDownloadedFeedForUpdates(xmlTitle,
													  xmlLink,
													  xmlReleaseNotesLink,
													  xmlPubDate,
													  xmlEnclosureUrl,
													  xmlEnclosureVersion,
													  xmlEnclosurePlatform,
													  xmlEnclosureLength,
													  xmlEnclosureType);

			}

		} else if (m_xml.isCharacters() && ! m_xml.isWhitespace()) {

			if (currentTag == QLatin1String("title")) {
				xmlTitle += m_xml.text().toString().trimmed();

			} else if (currentTag == QLatin1String("link")) {
				xmlLink += m_xml.text().toString().trimmed();

			} else if (currentQualifiedTag == QLatin1String("sparkle:releaseNotesLink")) {
				xmlReleaseNotesLink += m_xml.text().toString().trimmed();

			} else if (currentTag == QLatin1String("pubDate")) {
				xmlPubDate += m_xml.text().toString().trimmed();

			}

		}

		if (m_xml.error() && m_xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {

			showErrorDialog(tr("Feed parsing failed: %1 %2.").arg(QString::number(m_xml.lineNumber()), m_xml.errorString()), NO_UPDATE_MESSAGE);
			return false;

		}
	}

    // No updates were found if we're at this point
    // (not a single <item> element found)
    showInformationDialog(tr("Your software is up to date."), false);

	return false;
}


bool FvUpdater::searchDownloadedFeedForUpdates(const QString& xmlTitle,
											   QString xmlLink,
											   QString xmlReleaseNotesLink,
											   const QString& xmlPubDate,
											   const QString& xmlEnclosureUrl,
											   const QString& xmlEnclosureVersion,
											   const QString& xmlEnclosurePlatform,
											   unsigned long xmlEnclosureLength,
											   const QString& xmlEnclosureType)
{
    qDebug() << "Title:" << xmlTitle;
    qDebug() << "Link:" << xmlLink;
    qDebug() << "Release notes link:" << xmlReleaseNotesLink;
    qDebug() << "Pub. date:" << xmlPubDate;
    qDebug() << "Enclosure URL:" << xmlEnclosureUrl;
    qDebug() << "Enclosure version:" << xmlEnclosureVersion;
    qDebug() << "Enclosure platform:" << xmlEnclosurePlatform;
    qDebug() << "Enclosure length:" << xmlEnclosureLength;
    qDebug() << "Enclosure type:" << xmlEnclosureType;

	// Validate
	if (xmlReleaseNotesLink.isEmpty()) {
		if (xmlLink.isEmpty()) {
			showErrorDialog(tr("Feed error: \"release notes\" link is empty"), NO_UPDATE_MESSAGE);
			return false;
		} else {
			xmlReleaseNotesLink = xmlLink;
		}
	} else {
		xmlLink = xmlReleaseNotesLink;
	}
	if (! (xmlLink.startsWith(QStringLiteral("http://")) || xmlLink.startsWith(QStringLiteral("https://")))) {
		showErrorDialog(tr("Feed error: invalid \"release notes\" link"), NO_UPDATE_MESSAGE);
		return false;
	}
	if (xmlEnclosureUrl.isEmpty() || xmlEnclosureVersion.isEmpty() || xmlEnclosurePlatform.isEmpty()) {
        showErrorDialog(tr("Feed error: invalid \"enclosure\" with the download link"), NO_UPDATE_MESSAGE);
		return false;
	}

	// Relevant version?
	if (FVIgnoredVersions::VersionIsIgnored(xmlEnclosureVersion)) {
		qDebug() << "Version '" << xmlEnclosureVersion << "' is ignored, too old or something like that.";

		showInformationDialog(tr("Your software is up to date."), false);

		return true;	// Things have succeeded when you think of it.
	}


	//
	// Success! At this point, we have found an update that can be proposed
	// to the user.
	//

	if (m_proposedUpdate) {
		delete m_proposedUpdate;
		m_proposedUpdate = nullptr;
	}

	governikus::ReleaseNotesDownloader downloader;

	m_proposedUpdate = new FvAvailableUpdate();
	m_proposedUpdate->SetTitle(xmlTitle);
	m_proposedUpdate->SetReleaseNotesLink(xmlReleaseNotesLink);
	m_proposedUpdate->SetReleaseNotes(downloader.loadReleaseNotes(m_proposedUpdate->GetReleaseNotesLink()));
	m_proposedUpdate->SetPubDate(xmlPubDate);
	m_proposedUpdate->SetEnclosureUrl(xmlEnclosureUrl);
	m_proposedUpdate->SetEnclosureVersion(xmlEnclosureVersion);
	m_proposedUpdate->SetEnclosurePlatform(xmlEnclosurePlatform);
	m_proposedUpdate->SetEnclosureLength(xmlEnclosureLength);
	m_proposedUpdate->SetEnclosureType(xmlEnclosureType);

	// Show "look, there's an update" window
	showUpdaterWindowUpdatedWithCurrentUpdateProposal();

	return true;
}


void FvUpdater::showErrorDialog(QString message, msgType type)
{
	if (m_silentAsMuchAsItCouldGet) {
		if (type != CRITICAL_MESSAGE) {
			// Don't show errors in the silent mode
			return;
		}
	}
	else
  {
    if(type == NO_UPDATE_MESSAGE)
    {
      message = tr("Your software is up to date.");
    }
  }

	QMessageBox dlFailedMsgBox(QApplication::activeWindow());
	dlFailedMsgBox.setIcon(QMessageBox::Critical);
	dlFailedMsgBox.setWindowTitle(QApplication::applicationName() + " - "+ tr("Error"));
	dlFailedMsgBox.setWindowIcon(QIcon(QStringLiteral(":/images/autentapp2.iconset/icon_16x16.png")));
	dlFailedMsgBox.setWindowModality(Qt::WindowModal);
	dlFailedMsgBox.setWindowFlags(dlFailedMsgBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	dlFailedMsgBox.setText(message);
	dlFailedMsgBox.exec();
}

void FvUpdater::showInformationDialog(const QString& message, bool showEvenInSilentMode)
{
	if (m_silentAsMuchAsItCouldGet) {
		if (! showEvenInSilentMode) {
			// Don't show information dialogs in the silent mode
			return;
		}
	}

	QMessageBox dlInformationMsgBox(QApplication::activeWindow());
	dlInformationMsgBox.setIcon(QMessageBox::Information);
	dlInformationMsgBox.setWindowTitle(QApplication::applicationName() + " - "+tr("Information"));
	dlInformationMsgBox.setWindowIcon(QIcon(QStringLiteral(":/images/autentapp2.iconset/icon_16x16.png")));
	dlInformationMsgBox.setWindowModality(Qt::WindowModal);
	dlInformationMsgBox.setWindowFlags(dlInformationMsgBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	dlInformationMsgBox.setText(message);
	dlInformationMsgBox.exec();
}

