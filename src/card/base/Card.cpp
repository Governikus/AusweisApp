/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "Card.h"

#include "VolatileSettings.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;

Card::Card()
	: QObject()
{
}


void Card::setProgressMessage(const QString& pMessage, int pProgress)
{
	Q_UNUSED(pMessage)
	Q_UNUSED(pProgress)
}


void Card::setErrorMessage(const QString& pMessage)
{
	Q_UNUSED(pMessage)
}


EstablishPaceChannelOutput Card::establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription, quint8 pTimeoutSeconds)
{
	Q_UNUSED(pPasswordId)
	Q_UNUSED(pPreferredPinLength)
	Q_UNUSED(pChat)
	Q_UNUSED(pCertificateDescription)
	Q_UNUSED(pTimeoutSeconds)
	qCWarning(card) << "Establishment of PACE channel not supported";
	return EstablishPaceChannelOutput(CardReturnCode::COMMAND_FAILED);
}


CardReturnCode Card::destroyPaceChannel()
{
	qCWarning(card) << "Destruction of PACE channel not supported";
	return CardReturnCode::COMMAND_FAILED;
}


ResponseApduResult Card::setEidPin(quint8 pTimeoutSeconds)
{
	Q_UNUSED(pTimeoutSeconds)
	qCWarning(card) << "Setting eID PIN is not supported";

	return {CardReturnCode::COMMAND_FAILED};
}


QString Card::generateProgressMessage(const QString& pMessage, int pProgress)
{
	const bool usedAsSdk = Env::getSingleton<VolatileSettings>()->isUsedAsSDK();

	QString message = usedAsSdk
			? Env::getSingleton<VolatileSettings>()->getMessages().getSessionInProgress()
			: pMessage;

	if (pProgress != -1 || usedAsSdk)
	{
		if (!message.isEmpty())
		{
			message += QLatin1Char('\n');
		}
		message += QStringLiteral("%1 %").arg(pProgress > 0 ? pProgress : 0);
	}

	return message;
}


QString Card::generateErrorMessage(const QString& pMessage)
{
	return Env::getSingleton<VolatileSettings>()->isUsedAsSDK() && !pMessage.isNull()
			? Env::getSingleton<VolatileSettings>()->getMessages().getSessionFailed()
			: pMessage;
}


EstablishPaceChannelOutput Card::prepareIdentification(const QByteArray& pChat)
{
	Q_UNUSED(pChat)
	qCWarning(card) << "Preparing identification is not supported";

	return EstablishPaceChannelOutput();
}


ResponseApduResult Card::getChallenge()
{
	qCWarning(card) << "Getting challenge is not supported";

	return {CardReturnCode::COMMAND_FAILED};
}


TerminalAndChipAuthenticationResult Card::performTAandCA(
		const CVCertificateChain& pTerminalCvcChain,
		const QByteArray& pAuxiliaryData,
		const QByteArray& pSignature,
		const QByteArray& pPin,
		const QByteArray& pEphemeralPublicKey)
{
	Q_UNUSED(pTerminalCvcChain)
	Q_UNUSED(pAuxiliaryData)
	Q_UNUSED(pSignature)
	Q_UNUSED(pPin)
	Q_UNUSED(pEphemeralPublicKey)
	qCWarning(card) << "Performing terminal and card authentication is not supported";

	return {CardReturnCode::COMMAND_FAILED};
}
