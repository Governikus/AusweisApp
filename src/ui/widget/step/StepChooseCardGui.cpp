/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StepChooseCardGui.h"

#include "HelpAction.h"
#include "ReaderConfiguration.h"
#include "RemoteClient.h"
#include "step/AuthenticateStepsWidget.h"

#include <QLoggingCategory>
#include <QPushButton>
#include <QSharedPointer>
#include <QVector>
#include <QWidget>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


StepChooseCardGui::StepChooseCardGui(const QSharedPointer<WorkflowContext>& pContext, QWidget* pParent)
	: StepGui(pContext)
	, mContext(pContext)
	, mAuthWidget(nullptr)
	, mInformationMessageBox(new QMessageBox(pParent))
	, mReaderDeviceGui(new ReaderDeviceGui(pParent))
	, mCancelButton(nullptr)
	, mDeviceButton(nullptr)
	, mSubDialogOpen(false)
{
	const auto& widget = qobject_cast<AuthenticateStepsWidget*>(pParent);
	if (widget)
	{
		mAuthWidget = widget->getEac1Page();
	}

	mInformationMessageBox->setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
	mInformationMessageBox->setWindowModality(Qt::WindowModal);
	mInformationMessageBox->setWindowFlags(mInformationMessageBox->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	mCancelButton = mInformationMessageBox->addButton(tr("Cancel"), QMessageBox::NoRole);
	mDeviceButton = mInformationMessageBox->addButton(tr("Settings"), QMessageBox::YesRole);
	mDeviceButton->setFocus();

	connect(mReaderDeviceGui.data(), &ReaderDeviceGui::fireFinished, this, &StepChooseCardGui::onSubDialogFinished);
}


StepChooseCardGui::~StepChooseCardGui()
{
}


void StepChooseCardGui::activate()
{
	if (mAuthWidget)
	{
		mAuthWidget->setContext(mContext.objectCast<AuthContext>());
	}

	setCancelButtonState(ButtonState::ENABLED);

	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderEvent, this, &StepChooseCardGui::onReaderManagerSignal);
	onReaderManagerSignal();
}


void StepChooseCardGui::deactivate()
{
	if (mAuthWidget)
	{
		mAuthWidget->setContext(QSharedPointer<AuthContext>());
	}

	disconnect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderEvent, this, &StepChooseCardGui::onReaderManagerSignal);
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
	Env::getSingleton<RemoteClient>()->startDetection();

	QMetaObject::invokeMethod(this, &StepChooseCardGui::onReaderManagerSignal, Qt::QueuedConnection);
}


const QString StepChooseCardGui::connectedRemoteReaderNames() const
{
	RemoteClient* remoteClient = Env::getSingleton<RemoteClient>();
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
	const auto readers = Env::getSingleton<ReaderManager>()->getReaderInfos();

	mReaderDeviceGui->reactToReaderCount(readers.size());

	bool readerWithInsufficientApduLength = false;
	QVector<ReaderInfo> readersWithNpa;
	QVector<ReaderInfo> remoteReaders;
	for (const auto& readerInfo : readers)
	{
		if (!readerInfo.sufficientApduLength())
		{
			readerWithInsufficientApduLength = true;
		}
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
		if (readerWithInsufficientApduLength)
		{
			if (readers.size() == 1)
			{
				updateErrorMessage(tr("Extended Length is not supported."),
						tr("Your remote reader does not meet the technical requirements (Extended Length not supported)."),
						QString(),
						false);
				return;
			}
			else
			{
				updateErrorMessage(tr("Extended Length is not supported."),
						tr("At least one of your card readers does not meet the technical requirements (Extended Length not supported). Please place the ID card on a different card reader."),
						QString(),
						false);
			}
		}
		else
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
		if (readersWithNpa[0].isPinDeactivated() && !mContext->isCanAllowedMode())
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
