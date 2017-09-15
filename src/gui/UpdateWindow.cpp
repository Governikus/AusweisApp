/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "UpdateWindow.h"

#include "ui_UpdateWindow.h"
#include "Updater.h"

#include <QApplication>
#include <QDesktopServices>

using namespace governikus;

UpdateWindow::UpdateWindow(bool pSilent, QWidget* pParent)
	: QDialog(pParent)
	, mUi()
	, mSilent(pSilent)
	, mUpdateData()
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	auto& updater = Updater::getInstance();
	connect(&updater, &Updater::fireAppUpdateCheckFinished, this, &UpdateWindow::onAppUpdateCheckFinished);
	updater.checkAppUpdate();
}


UpdateWindow::~UpdateWindow()
{
}


void UpdateWindow::onAppUpdateCheckFinished(bool pUpdateAvailable, const GlobalStatus& pError)
{
	if (pError.isError())
	{
		showBox(QMessageBox::Critical, pError.toErrorDescription());
	}
	else if (pUpdateAvailable)
	{
		mUpdateData = Updater::getInstance().getUpdateData();
		fillData();
		show();
		return;
	}
	else if (!mSilent)
	{
		showBox(QMessageBox::Information, tr("Your software is up to date."));
	}

	close();
}


void UpdateWindow::onUpdateClicked()
{
	const auto& url = mUpdateData.getUrl();
	qDebug() << "Download application update:" << url;
	if (!QDesktopServices::openUrl(url))
	{
		showBox(QMessageBox::Warning, tr("Unable to open this link in a browser. Please copy and paste the link into the address bar of your browser."));
	}
	close();
}


void UpdateWindow::onSkipVersionClicked()
{
	Updater::getInstance().skipVersion(mUpdateData.getVersion());
	close();
}


void UpdateWindow::showBox(QMessageBox::Icon pIcon, const QString& pMsg)
{
	QMessageBox box(QApplication::activeWindow());
	box.setIcon(pIcon);
	box.setWindowTitle(QApplication::applicationName() + QStringLiteral(" - ") + tr("Updates"));
	box.setWindowIcon(QIcon(QStringLiteral(":/images/npa.svg")));
	box.setWindowModality(Qt::WindowModal);
	box.setText(pMsg);
	box.exec();
}


void UpdateWindow::fillData()
{
	if (!mUi)
	{
		mUi.reset(new Ui::UpdateWindow());
		mUi->setupUi(this);

		connect(mUi->skipButton, &QAbstractButton::clicked, this, &UpdateWindow::onSkipVersionClicked);
		connect(mUi->reminderButton, &QAbstractButton::clicked, this, &QWidget::close);
		connect(mUi->downloadButton, &QAbstractButton::clicked, this, &UpdateWindow::onUpdateClicked);
	}

	mUi->downloadLabel->setText(mUi->downloadLabel->text().arg(mUpdateData.getVersion(), QApplication::applicationVersion()));
	mUi->linkLabel->setText(mUi->linkLabel->text().arg(mUpdateData.getUrl().toString()));
	mUi->releaseNotes->setHtml(mUpdateData.getNotes().isEmpty() ? tr("<h4>Download of release notes failed</h4>") : mUpdateData.getNotes());
	mUi->releaseNotes->setAccessibleName(mUi->releaseNotes->toPlainText());
}
