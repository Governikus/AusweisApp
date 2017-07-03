/*!
 * ReturnCodeUtil.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ReturnCodeUtil.h"

#include <QHash>

using namespace governikus;

const ReturnCodeUtil::MapType ReturnCodeUtil::mCodeTable
{
	{
		ReturnCode::OK,
		{
			Result::Minor::AL_Unknown_Error,
			QT_TR_NOOP("No error occurred."),
			QT_TR_NOOP("no error")
		}
	},
	{
		ReturnCode::NO_CARD,
		{
			Result::Minor::AL_Unknown_Error,
			QT_TR_NOOP("Card does not exist"),
			QT_TR_NOOP("Card does not exist")
		}
	},
	{
		ReturnCode::TIME_OUT,
		{
			Result::Minor::AL_Unknown_Error,
			QT_TR_NOOP("The maximum time was exceeded during input process."),
			QT_TR_NOOP("A timeout occurred during input process")
		}
	},
	{
		ReturnCode::UNKNOWN,
		{
			Result::Minor::AL_Unknown_Error,
			QT_TR_NOOP("The input is invalid."),
			QT_TR_NOOP("invalid input")
		}
	},
	{
		ReturnCode::COMMAND_FAILED,
		{
			Result::Minor::AL_Unknown_Error,
			QT_TR_NOOP("An error occurred while communicating with the card reader."),
			QT_TR_NOOP("An error occurred while communicating with the card reader.")
		}
	},
	{
		ReturnCode::GET_CHALLENGE_FAILED,
		{
			Result::Minor::AL_Unknown_Error,
			QT_TR_NOOP("An error occurred while communicating with the card reader."),
			QT_TR_NOOP("An error occurred while communicating with the card reader.")
		}
	},
	{
		ReturnCode::CANCELLATION_BY_USER,
		{
			Result::Minor::SAL_Cancellation_by_User,
			QT_TR_NOOP("The process was cancelled by the user."),
			QT_TR_NOOP("The process was cancelled by the user.")
		}
	},
	{
		ReturnCode::NEW_PINS_DONT_MATCH,
		{
			Result::Minor::AL_Unknown_Error,
			QT_TR_NOOP("The new PIN and the confirmation do not match."),
			QT_TR_NOOP("The new PIN and the confirmation do not match.")
		}
	},
	{
		ReturnCode::NEW_PIN_TOO_SHORT_OR_LONG,
		{
			Result::Minor::AL_Unknown_Error,
			QT_TR_NOOP("The length of the new PIN is not valid."),
			QT_TR_NOOP("The length of the new PIN is not valid.")
		}
	},
	{
		ReturnCode::PIN_BLOCKED,
		{
			Result::Minor::AL_Unknown_Error,
			QT_TR_NOOP("The PIN was blocked after too many unsuccessful attempts."),
			QT_TR_NOOP("PIN is blocked")
		}
	},
	{
		ReturnCode::PROTOCOL_ERROR,
		{
			Result::Minor::AL_Internal_Error,
			QT_TR_NOOP("A protocol error has occurred."),
			QT_TR_NOOP("A protocol error has occurred.")
		}
	},
	{
		ReturnCode::PIN_NOT_BLOCKED,
		{
			Result::Minor::AL_Unknown_Error,
			QT_TR_NOOP("The PIN is not blocked."),
			QT_TR_NOOP("The PIN is not blocked.")
		}
	},
	{
		ReturnCode::UNEXPECTED_TRANSMIT_STATUS,
		{
			Result::Minor::AL_Internal_Error,
			QT_TR_NOOP("Unexpected transmit status"),
			QT_TR_NOOP("Unexpected transmit status")
		}
	},
	{
		ReturnCode::CAN_INVALID,
		{
			Result::Minor::AL_Communication_Error,
			QT_TR_NOOP("The given card access number (CAN) is invalid."),
			QT_TR_NOOP("given card access number (CAN) is invalid")
		}
	},
	{
		ReturnCode::PIN_INVALID,
		{
			Result::Minor::AL_Communication_Error,
			QT_TR_NOOP("The given PIN is invalid."),
			QT_TR_NOOP("given PIN is invalid")
		}
	},
	{
		ReturnCode::PUK_INVALID,
		{
			Result::Minor::AL_Communication_Error,
			QT_TR_NOOP("The given PUK is invalid."),
			QT_TR_NOOP("given PUK is invalid")
		}
	},
	{
		ReturnCode::PUK_INOPERATIVE,
		{
			Result::Minor::AL_Communication_Error,
			QT_TR_NOOP("The PUK was used ten times and is set inoperative."),
			QT_TR_NOOP("PUK is inoperative")
		}
	},
};


QString ReturnCodeUtil::toString(ReturnCode pCode)
{
	auto i = mCodeTable.find(pCode);
	if (i == mCodeTable.end())
	{
		return tr("Unknown error");
	}
	return tr(i->shortText);
}


QString ReturnCodeUtil::toMessage(ReturnCode pCode)
{
	auto i = mCodeTable.find(pCode);
	if (i == mCodeTable.end())
	{
		return tr("An unknown error occurred: ") + pCode;
	}

	return tr(i->longText);
}


Result::Minor ReturnCodeUtil::toResult(ReturnCode pCode)
{
	auto i = mCodeTable.find(pCode);
	if (i == mCodeTable.end())
	{
		return Result::Minor::AL_Unknown_Error;
	}
	return i->trCode;
}


QPair<Result::Minor, QString> ReturnCodeUtil::toError(ReturnCode pCode)
{
	auto i = mCodeTable.find(pCode);
	if (i == mCodeTable.end())
	{
		return QPair<Result::Minor, QString>(Result::Minor::AL_Unknown_Error,
				tr("An unknown error occurred: ") + pCode);
	}
	return QPair<Result::Minor, QString>(i->trCode, tr(i->longText));
}
