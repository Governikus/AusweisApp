/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerInsertCard.h"

#include "ReaderManager.h"

using namespace governikus;

MsgHandlerInsertCard::MsgHandlerInsertCard()
	: MsgHandler(MsgType::INSERT_CARD)
{
	const auto& infos = Env::getSingleton<ReaderManager>()->getReaderInfos();
	for (const auto& entry : infos)
	{
		if (entry.hasEidCard())
		{
			setVoid();
			break;
		}
	}
}


MsgHandlerInsertCard::MsgHandlerInsertCard(MsgContext& pContext)
	: MsgHandlerInsertCard()
{
	Q_ASSERT(pContext.getContext());
	pContext.getContext()->setStateApproved();
}
