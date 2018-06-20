/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StepChooseCardGui.h"

#include "Env.h"
#include "generic/HelpAction.h"
#include "GuiProfile.h"
#include "ReaderConfiguration.h"
#include "step/AuthenticateStepsWidget.h"

#include <QLoggingCategory>
#include <QPushButton>
#include <QSharedPointer>
#include <QVector>
#include <QWidget>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


StepChooseCardGui::StepChooseCardGui(const QSharedPointer<AuthContext>& pContext, AuthenticateStepsWidget* pStepsWidget)
	: StepGui(pContext)
	, mContext(pContext)
	, mWidget(pStepsWidget->getEac1Page())
	, mInformationMessageBox(new QMessageBox(pStepsWidget))
	, mReaderDeviceGui(new ReaderDeviceGui(pStepsWidget))
	, mCancelButton(nullptr)
	, mDeviceButton(nullptr)
	, mSubDialogOpen(false)
{
	mInformationMessageBox->setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
	mInformationMessageBox->setWindowModality(Qt::WindowModal);
	mInformationMessageBox->setWindowFlags(mInformationMessageBox->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	mCancelButton = mInformationMessageBox->addButton(tr("Cancel"), QMessageBox::NoRole);
	mDeviceButton = mInformationMessageBox->addButton(tr("Settings"), QMessageBox::YesRole);
	mDeviceButton->setFocus();

	connect(mReaderDeviceGui, &ReaderDeviceGui::fireFinished, this, &StepChooseCardGui::onSubDialogFinished);
}


StepChooseCardGui::~StepChooseCardGui()
{
}


void StepChooseCardGui::activate()
{
	mWidget->setContext(mContext);

	setCancelButtonState(ButtonState::ENABLED);

	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderEvent, this, &StepChooseCardGui::onReaderManagerSignal);
	onReaderManagerSignal();
}


void StepChooseCardGui::deactivate()
{
	mWidget->setContext(QSharedPointer<AuthContext>());

	disconnect(&ReaderManager::getInstance(), &ReaderManager::fireReaderEvent, this, &StepChooseCardGui::onReaderManagerSignal);
}


QString StepChooseCardGui::getCurrentReaderImage(const QVector<ReaderInfo>& pReaderInfos)
{
	if (pReaderInfos.size() == 1)
	{
		return pReaderInfos.at(0).getReaderConfigurationInfo().getIcon()->lookupPath();
	}
	else if (pReaderInfos.size() > 1)
	{
		return ReaderConfiguration::getMultipleReaderIconPath();
	}

	return ReaderConfiguration::getNoReaderFoundIconPath();
}


QString StepChooseCardGui::formatErrorMessages(const QString& pMessage1, const QString& pMessage2)
{
	const bool oneMessageIsEmpty = pMessage1.isEmpty() || pMessage2.isEmpty();

	return oneMessageIsEmpty ? pMessage1 + pMessage2 : QStringLiteral("<p>%1</p><p>%2</p>").arg(pMessage1, pMessage2);
}


void StepChooseCardGui::updateErrorMessage(const QString& pTitle, const QString& pMessage1, const QString& pMessage2, bool closeErrorMessage)
{
	if (closeErrorMessage || mContext->getStatus().isError())
	{
		mReaderDeviceGui->deactivate();
		mInformationMessageBox->done(QMessageBox::InvalidRole);
		return;
	}

	QString iconPath = getCurrentReaderImage(Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter::UniqueReaderTypes));
	if (iconPath.isEmpty())
	{
		mInformationMessageBox->setIcon(QMessageBox::Information);
	}
	else
	{
		mInformationMessageBox->setIconPixmap(QPixmap(iconPath).scaledToWidth(200, Qt::SmoothTransformation));
	}
	mInformationMessageBox->setText(QStringLiteral("<b>%1</b>").arg(pTitle));
	mInformationMessageBox->setInformativeText(formatErrorMessages(pMessage1, pMessage2));

	if (mInformationMessageBox->isVisible() || mSubDialogOpen)
	{
		return;
	}

	if (mInformationMessageBox->exec() != QMessageBox::InvalidRole)
	{
		if (mInformationMessageBox->clickedButton() == mCancelButton)
		{
			Q_EMIT fireCancelled();
			return;
		}

		mSubDialogOpen = true;
		if (mInformationMessageBox->clickedButton() == mDeviceButton)
		{
			mReaderDeviceGui->activate();
		}
	}
	// else: dialog was closed by an onErrorMessage(..., true) call (i.e. card found)
}


void StepChooseCardGui::onSubDialogFinished()
{
	mSubDialogOpen = false;
	const QSharedPointer<RemoteClient>& remoteClient = ReaderManager::getInstance().getRemoteClient();
	remoteClient->startDetection();
	QMetaObject::invokeMethod(this, "onReaderManagerSignal", Qt::QueuedConnection);
}


const QString StepChooseCardGui::connectedRemoteReaderNames() const
{
	const QSharedPointer<RemoteClient>& remoteClient = Env::getSingleton<ReaderManager>()->getRemoteClient();
	const auto deviceInfos = remoteClient->getConnectedDeviceInfos();
	QStringList deviceNames;
	for (const auto& info : deviceInfos)
	{
		deviceNames.append(QLatin1Char('"') + info.getName() + QLatin1Char('"'));
	}
	return deviceNames.join(QLatin1String(", "));
}


void StepChooseCardGui::onReaderManagerSignal()
{
	const auto readers = ReaderManager::getInstance().getReaderInfos();

	mReaderDeviceGui->reactToReaderCount(readers.size());

	QVector<ReaderInfo> readersWithNpa;
	QVector<ReaderInfo> remoteReaders;
	for (const auto& readerInfo : readers)
	{
		if (readerInfo.hasEidCard())
		{
			readersWithNpa << readerInfo;
		}
		if (readerInfo.getPlugInType() == ReaderManagerPlugInType::REMOTE)
		{
			remoteReaders << readerInfo;
		}
	}

	if (readers.size() == 0)
	{
		const QString onlineHelpUrl = HelpAction::getOnlineUrl(QStringLiteral("stepChooseCardGui"));
		const QString onlineHelpText = tr("If you need help or have problems with your card reader, you can consult the "
										  "%1online help%2 for futher information.")
				.arg(QStringLiteral("<a href=\"%1\">").arg(onlineHelpUrl), QStringLiteral("</a>"));

		updateErrorMessage(tr("No card reader found. Please connect card reader first."),
				tr("Please choose \"Settings\" to install a card reader or configure your smartphone as a card reader."),
				onlineHelpText,
				false);
		return;
	}

	if (readersWithNpa.size() == 0)
	{
		QString remoteReaderInfo;
		if (remoteReaders.size() > 0)
		{
			remoteReaderInfo = tr("Connected to following remote readers: %1.").arg(connectedRemoteReaderNames());
		}
		const QString onlineHelpUrl = HelpAction::getOnlineUrl(QStringLiteral("stepChooseCardGui"));
		const QString onlineHelpText = tr("If you have already placed an ID card on your card reader, "
										  "you can consult the %1online help%2 for futher information.")
				.arg(QStringLiteral("<a href=\"%1\">").arg(onlineHelpUrl), QStringLiteral("</a>"));

		updateErrorMessage(tr("Please place an ID card on the card reader."), onlineHelpText, remoteReaderInfo, false);
	}
	else if (readersWithNpa.size() > 1)
	{
		const QString onlineHelpUrl = HelpAction::getOnlineUrl(QStringLiteral("stepChooseCardGui"));
		const QString onlineHelpText = tr("Please make sure that only one card reader with an ID card on it is connected to "
										  "your computer. If you have already placed an ID card on your card reader, "
										  "you can consult the %1online help%2 for futher information.")
				.arg(QStringLiteral("<a href=\"%1\">").arg(onlineHelpUrl), QStringLiteral("</a>"));

		updateErrorMessage(tr("Please place only one ID card on the card reader."), onlineHelpText, QString(), false);
	}
	else
	{
		if (!readersWithNpa[0].sufficientApduLength())
		{
			updateErrorMessage(tr("Extended Length is not supported."),
					tr("Your remote reader does not meet the technical requirements (Extended Length not supported)."),
					QString(),
					false);
		}
		else if (readersWithNpa[0].isPinDeactivated() && !mContext->isCanAllowedMode())
		{
			updateErrorMessage(tr("Online identification function is disabled."),
					tr("This action cannot be performed. The online identification function of your ID card is deactivated."
					   " Please contact the authority responsible for issuing your identification document to activate the online identification function."),
					QString(),
					false);
		}
		else
		{
			updateErrorMessage(QString(), QString(), QString(), true);
		}
	}
}
