/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "MockIfdStatus.h"


using namespace governikus;


namespace
{
VALUE_NAME(MSG_TYPE, "msg")
VALUE_NAME(CONTEXT_HANDLE, "ContextHandle")
VALUE_NAME(SLOT_NAME, "SlotName")
VALUE_NAME(PIN_CAPABILITIES, "PINCapabilities")
VALUE_NAME(MAX_APDU_LENGTH, "MaxAPDULength")
VALUE_NAME(CONNECTED_READER, "ConnectedReader")
VALUE_NAME(CARD_AVAILABLE, "CardAvailable")
VALUE_NAME(EF_ATR, "EFATR")
VALUE_NAME(EF_DIR, "EFDIR")
}


MockIfdStatus::MockIfdStatus(const QString& pSlotName,
		const PaceCapabilities& pPaceCapabilities,
		int pMaxApduLength,
		bool pConnected,
		bool pCardAvailable)
	: IfdStatus(createJsonObject(
				pSlotName,
				pPaceCapabilities,
				pMaxApduLength,
				pConnected,
				pCardAvailable))
{
}


MockIfdStatus::~MockIfdStatus()
{
}


QJsonObject MockIfdStatus::createJsonObject(const QString& pSlotName,
		const PaceCapabilities& pPaceCapabilities,
		int pMaxApduLength,
		bool pConnectedReader,
		bool pCardAvailable)
{
	QJsonObject result;
	result[MSG_TYPE()] = getEnumName(RemoteCardMessageType::IFDStatus);
	result[CONTEXT_HANDLE()] = QStringLiteral("contextHandle");
	result[SLOT_NAME()] = pSlotName;
	result[PIN_CAPABILITIES()] = pPaceCapabilities.toJson();
	result[MAX_APDU_LENGTH()] = pMaxApduLength;
	result[CONNECTED_READER()] = pConnectedReader;
	result[CARD_AVAILABLE()] = pCardAvailable;
	result[EF_ATR()] = QJsonValue();
	result[EF_DIR()] = QJsonValue();
	return result;
}
