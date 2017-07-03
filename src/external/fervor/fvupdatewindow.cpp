#include "fvupdatewindow.h"
#include "ui_fvupdatewindow.h"
#include "fvupdater.h"
#include "fvavailableupdate.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>


FvUpdateWindow::FvUpdateWindow(QWidget *pParent)
	: QWidget(pParent)
	, m_ui(new Ui::FvUpdateWindow)
	, m_appIconScene()
{
	m_ui->setupUi(this);

	m_appIconScene = nullptr;

	// Delete on close
	setAttribute(Qt::WA_DeleteOnClose, true);

	// Set the "close app, then reopen" string
	QString closeReopenString = m_ui->downloadThisUpdateLabel->text().arg(QApplication::applicationName());
	m_ui->downloadThisUpdateLabel->setText(closeReopenString);

	// Set the "new version is available" string
	QString newVersString = m_ui->newVersionIsAvailableLabel->text().arg(QApplication::applicationName());
	m_ui->newVersionIsAvailableLabel->setText(newVersString);

	// Connect buttons
	connect(m_ui->installUpdateButton, &QPushButton::clicked, FvUpdater::sharedUpdater(), &FvUpdater::InstallUpdate);
	connect(m_ui->skipThisVersionButton, &QPushButton::clicked, FvUpdater::sharedUpdater(), &FvUpdater::SkipUpdate);
	connect(m_ui->remindMeLaterButton, &QPushButton::clicked, FvUpdater::sharedUpdater(), &FvUpdater::RemindMeLater);
}

FvUpdateWindow::~FvUpdateWindow()
{
	delete m_ui;
}

bool FvUpdateWindow::UpdateWindowWithCurrentProposedUpdate()
{
	FvAvailableUpdate* proposedUpdate = FvUpdater::sharedUpdater()->GetProposedUpdate();
	if (! proposedUpdate) {
		return false;
	}

	QString downloadString = m_ui->wouldYouLikeToDownloadLabel->text()
			.arg(QApplication::applicationName(), proposedUpdate->GetEnclosureVersion(), QApplication::applicationVersion());
	m_ui->wouldYouLikeToDownloadLabel->setText(downloadString);

	QString downloadLinkString = m_ui->updateFileLinkLabel->text().arg(proposedUpdate->GetEnclosureUrl().toString());
	m_ui->updateFileLinkLabel->setText(downloadLinkString);

	m_ui->releaseNotesWebView->setHtml(proposedUpdate->GetReleaseNotes());
	m_ui->releaseNotesWebView->setAccessibleName(m_ui->releaseNotesWebView->toPlainText());

	return true;
}

void FvUpdateWindow::closeEvent(QCloseEvent* pEvent)
{
	FvUpdater::sharedUpdater()->updaterWindowWasClosed();
	pEvent->accept();
}
