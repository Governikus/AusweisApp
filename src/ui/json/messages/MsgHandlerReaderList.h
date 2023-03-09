/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Message ReaderList of JSON API.
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerReaderList
	: public MsgHandler
{
	public:
		explicit MsgHandlerReaderList(const MsgContext& pContext);
};


} // namespace governikus
