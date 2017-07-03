/*!
 * Card.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */
#include "Card.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;

Card::Card()
	: QObject()
{
}


Card::~Card()
{
}


ReturnCode Card::destroyPaceChannel()
{
	qCWarning(card) << "Destruction of PACE channel not supported";
	return ReturnCode::COMMAND_FAILED;
}
