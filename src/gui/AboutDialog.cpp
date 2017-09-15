/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#include "AboutDialog.h"
#include "ui_AboutDialog.h"

#include "AppSettings.h"
#include "BuildHelper.h"

using namespace governikus;


AboutDialog::AboutDialog(QWidget* pParent)
	: QDialog(pParent)
	, mUi(new Ui::AboutDialog)
{
	mUi->setupUi(this);

	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	layout()->setSizeConstraint(QLayout::SetFixedSize); // For platform != Windows: Disable maximize button
	setWindowModality(Qt::WindowModal); // For platform == macOS: Make dialog slide in from the top

	setWindowTitle(tr("About %1").arg(QCoreApplication::applicationName()));
	mUi->lblFurtherInformation->setText(tr("<b>Further information:</b> %1")
			.arg(QStringLiteral("<a href='https://www.ausweisapp.bund.de/'>https://www.ausweisapp.bund.de/</a>")));
	mUi->lblReleaseNotes->setText(tr("The current release notes can be found %1 here. %2")
			.arg(QStringLiteral("<a href='https://appl.governikus-asp.de/ausweisapp2/ReleaseNotes.html'>"), QStringLiteral("</a>")));
	mUi->lblVersion->setText(tr("<b>Version:</b> %1 (%2)").arg(QApplication::applicationVersion(), QString::fromLatin1(BuildHelper::getDateTime())));
	mUi->lblDeveloperModeWarning->setText(QStringLiteral("<html><head/><body><p><span style='color:red;'>%1</span></p></body></html>")
			.arg(tr("The developer mode is aimed at integrators / developers for new service applications. For this reason, the developer mode works only in the test PKI. By activating the developer mode, some safety tests are deactivated. This means that the authentication process continues although the AusweisApp2 would usually abort the process with an error message when used in normal operation mode. Information on the disregarded error in the developer mode is displayed in the attached window below the AusweisApp2.")));

	const QIcon icon = windowIcon();
	const QSize size = icon.actualSize(QSize(64, 64));
	mUi->imgAusweisApp2->setPixmap(icon.pixmap(size));

	connect(mUi->btnOkay, &QPushButton::clicked, this, &QDialog::accept);
	connect(this, &QDialog::accepted, this, &AboutDialog::onAccept);

	mUi->chkbDeveloperMode->setCheckState(AppSettings::getInstance().getGeneralSettings().isDeveloperMode() ? Qt::Checked : Qt::Unchecked);

	connect(mUi->chkbDeveloperMode, &QCheckBox::stateChanged, this, &AboutDialog::onCheckboxStateChanged);
	onCheckboxStateChanged();
}


AboutDialog::~AboutDialog()
{
}


void AboutDialog::onCheckboxStateChanged()
{
	const bool developerModeActivated = mUi->chkbDeveloperMode->checkState() == Qt::Checked;
	mUi->lblDeveloperModeWarning->setVisible(developerModeActivated);
	resize(minimumSize());
	adjustSize();
}


void AboutDialog::onAccept()
{
	const bool developerModeActivated = mUi->chkbDeveloperMode->checkState() == Qt::Checked;
	GeneralSettings& generalSettings = AppSettings::getInstance().getGeneralSettings();
	if (generalSettings.isDeveloperMode() != developerModeActivated)
	{
		generalSettings.setDeveloperMode(developerModeActivated);
		generalSettings.save();
	}
}
