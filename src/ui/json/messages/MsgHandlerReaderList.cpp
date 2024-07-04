/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerReaderList.h"

#include "MsgHandlerReader.h"
#include "ReaderManager.h"

#include <QJsonArray>

using namespace governikus;

MsgHandlerReaderList::MsgHandlerReaderList(const MsgContext& pContext)
	: MsgHandler(MsgType::READER_LIST)
{
	QJsonArray reader;
	const auto& infoList = Env::getSingleton<ReaderManager>()->getReaderInfos();
	for (const auto& info : infoList)
	{
		reader += MsgHandlerReader::createReaderInfo(info, pContext);
	}

	const QLatin1String parameterName = pContext.getApiLevel() >= MsgLevel::v2 ? QLatin1String("readers") : QLatin1String("reader");
	setValue(parameterName, reader);
}
