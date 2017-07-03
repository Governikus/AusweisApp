/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerInsertCard.h"

#include "ReaderManager.h"

using namespace governikus;

MsgHandlerInsertCard::MsgHandlerInsertCard(MsgContext& pContext)
	: MsgHandler(MsgType::INSERT_CARD)
{
	Q_ASSERT(pContext.getWorkflowContext());
	pContext.getWorkflowContext()->setStateApproved();

	const auto& infos = ReaderManager::getInstance().getReaderInfos();
	for (const auto& entry : infos)
	{
		if (entry.getCardType() == CardType::EID_CARD)
		{
			setVoid();
			break;
		}
	}
}
