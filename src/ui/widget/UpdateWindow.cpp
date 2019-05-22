/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "UpdateWindow.h"

#include "ui_UpdateWindow.h"

#include "Service.h"

#include <QApplication>
#include <QDesktopServices>

using namespace governikus;

UpdateWindow::UpdateWindow(QWidget* pParent)
	: QDialog(pParent)
	, mUi()
	, mUpdateData()
{
	fillData();
}


UpdateWindow::~UpdateWindow()
{
}


void UpdateWindow::onUpdateClicked()
{
	const auto& url = mUpdateData.getUrl();
	qDebug() << "Download application update:" << url;
	if (!QDesktopServices::openUrl(url))
	{
		Q_EMIT fireShowUpdateDialog(QMessageBox::Warning, tr("Unable to open this link in a browser. Please copy and paste the link into the address bar of your browser."));
	}
	close();
}


void UpdateWindow::onSkipVersionClicked()
{
	Env::getSingleton<AppUpdater>()->skipVersion(mUpdateData.getVersion());
	close();
}


void UpdateWindow::fillData()
{
	mUpdateData = Env::getSingleton<Service>()->getUpdateData();
	if (!mUi)
	{
		mUi.reset(new Ui::UpdateWindow());
		mUi->setupUi(this);

		connect(mUi->skipButton, &QAbstractButton::clicked, this, &UpdateWindow::onSkipVersionClicked);
		connect(mUi->reminderButton, &QAbstractButton::clicked, this, &QWidget::close);
		connect(mUi->downloadButton, &QAbstractButton::clicked, this, &UpdateWindow::onUpdateClicked);
	}

	mUi->downloadLabel->setText(tr("AusweisApp2 %1 is now available - you have %2. Would you like to download it now?").arg(mUpdateData.getVersion(), QApplication::applicationVersion()));
	mUi->linkLabel->setText(QStringLiteral("<a href=\"%1\">%1</a>").arg(mUpdateData.getUrl().toString()));
	mUi->releaseNotes->setHtml(mUpdateData.getNotes().isEmpty() ? tr("<h4>Download of release notes failed</h4>") : mUpdateData.getNotes());
	mUi->releaseNotes->setAccessibleName(mUi->releaseNotes->toPlainText());
}


void UpdateWindow::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange && mUi)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
