/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "AboutDialog.h"
#include "ui_AboutDialog.h"

#include "AppSettings.h"
#include "BuildHelper.h"
#include "SecureStorage.h"
#include "VersionNumber.h"

using namespace governikus;


AboutDialog::AboutDialog(QWidget* pParent)
	: QDialog(pParent)
	, mUi(new Ui::AboutDialog)
{
	mUi->setupUi(this);

	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	layout()->setSizeConstraint(QLayout::SetFixedSize); // For platform != Windows: Disable maximize button
	setWindowModality(Qt::WindowModal); // For platform == macOS: Make dialog slide in from the top

	const auto* storage = Env::getSingleton<SecureStorage>();
	const auto& url = VersionNumber::getApplicationVersion().isDeveloperVersion() ? storage->getAppcastBetaUpdateUrl() : storage->getAppcastUpdateUrl();
	const auto& releaseNotes = url.adjusted(QUrl::RemoveFilename).toString() + QStringLiteral("ReleaseNotes.html");

	setWindowTitle(tr("About %1 - %2").arg(QCoreApplication::applicationName(), QCoreApplication::organizationName()));

	mUi->lblFurtherInformation->setText(QStringLiteral("<b>%1:</b> <a href='https://www.ausweisapp.bund.de/'>https://www.ausweisapp.bund.de/</a>")
			.arg(tr("Further information")));

	mUi->lblReleaseNotes->setText(tr("The current release notes can be found %1 here.%2")
			.arg(QStringLiteral("<a href='%1'>").arg(releaseNotes), QStringLiteral("</a>")));

	mUi->lblVersion->setText(QStringLiteral("<b>%1:</b> %2").arg(tr("Version"), QApplication::applicationVersion()));

	const QIcon icon = windowIcon();
	const QSize size = icon.actualSize(QSize(64, 64));
	mUi->imgAusweisApp2->setPixmap(icon.pixmap(size));

	connect(mUi->btnOkay, &QPushButton::clicked, this, &QDialog::accept);
	connect(this, &QDialog::accepted, this, &AboutDialog::onAccept);

	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireDeveloperOptionsChanged, this, &AboutDialog::onDeveloperOptionsChanged);
	onDeveloperOptionsChanged();
}


AboutDialog::~AboutDialog()
{
}


void AboutDialog::onDeveloperOptionsChanged()
{
	mUi->chkbDeveloperOptions->setCheckState(Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperOptions() ? Qt::Checked : Qt::Unchecked);
}


void AboutDialog::onAccept()
{
	const bool developerOptionsActivated = mUi->chkbDeveloperOptions->checkState() == Qt::Checked;
	GeneralSettings& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	generalSettings.setDeveloperOptions(developerOptionsActivated);
}


void AboutDialog::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
