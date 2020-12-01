/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerInsertCard.h"

#include "ReaderManager.h"

using namespace governikus;

MsgHandlerInsertCard::MsgHandlerInsertCard(MsgContext& pContext)
	: MsgHandler(MsgType::INSERT_CARD)
{
	Q_ASSERT(pContext.getContext());
	pContext.getContext()->setStateApproved();

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
