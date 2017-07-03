/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerReaderList.h"

#include "MsgHandlerReader.h"
#include "ReaderManager.h"

#include <QJsonArray>

using namespace governikus;

MsgHandlerReaderList::MsgHandlerReaderList()
	: MsgHandler(MsgType::READER_LIST)
{
	QJsonArray reader;
	const auto& infoList = ReaderManager::getInstance().getReaderInfos();
	for (const auto& info : infoList)
	{
		reader += MsgHandlerReader::createReaderInfo(info);
	}
	mJsonObject["reader"] = reader;
}
