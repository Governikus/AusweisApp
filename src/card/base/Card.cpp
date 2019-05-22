/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "Card.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;

Card::Card()
	: QObject()
{
}


CardReturnCode Card::establishPaceChannel(PacePasswordId pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPaceChannelOutput& pChannelOutput, quint8 pTimeoutSeconds)
{
	Q_UNUSED(pPasswordId);
	Q_UNUSED(pChat);
	Q_UNUSED(pCertificateDescription);
	Q_UNUSED(pChannelOutput);
	Q_UNUSED(pTimeoutSeconds);
	qCWarning(card) << "Establishment of PACE channel not supported";
	return CardReturnCode::COMMAND_FAILED;
}


CardReturnCode Card::destroyPaceChannel()
{
	qCWarning(card) << "Destruction of PACE channel not supported";
	return CardReturnCode::COMMAND_FAILED;
}


CardReturnCode Card::setEidPin(quint8 pTimeoutSeconds, ResponseApdu& pResponseApdu)
{
	Q_UNUSED(pTimeoutSeconds);
	Q_UNUSED(pResponseApdu);
	qCWarning(card) << "Setting eID PIN is not supported";

	return CardReturnCode::COMMAND_FAILED;
}
