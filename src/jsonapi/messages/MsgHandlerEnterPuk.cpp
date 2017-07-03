/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerEnterPuk.h"

#include "context/WorkflowContext.h"

using namespace governikus;

MsgHandlerEnterPuk::MsgHandlerEnterPuk(const MsgContext& pContext)
	: MsgHandlerEnterNumber(MsgType::ENTER_PUK, pContext)
{
}
