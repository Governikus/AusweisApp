/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
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
	const auto& infoList = Env::getSingleton<ReaderManager>()->getReaderInfos();
	for (const auto& info : infoList)
	{
		reader += MsgHandlerReader::createReaderInfo(info);
	}
	mJsonObject[QLatin1String("reader")] = reader;
}
