/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerApiLevel.h"

#include <QJsonArray>

using namespace governikus;

MsgHandlerApiLevel::MsgHandlerApiLevel(const MsgContext& pContext)
	: MsgHandler(MsgType::API_LEVEL)
{
	setCurrentLevel(pContext.getApiLevel());
	setAvailableLevel();
}


MsgHandlerApiLevel::MsgHandlerApiLevel(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandler(MsgType::API_LEVEL)
{
	const auto& jsonLevel = pObj[QLatin1String("level")];
	if (jsonLevel.isUndefined())
	{
		setError(QLatin1String("Level cannot be undefined"));
	}
	else if (!jsonLevel.isDouble())
	{
		setError(QLatin1String("Invalid level"));
	}
	else
	{
		const int level = jsonLevel.toInt();
		if (Enum<MsgLevel>::isValue(level))
		{
			pContext.setApiLevel(static_cast<MsgLevel>(level));
		}
		else
		{
			setError(QLatin1String("Unknown level"));
		}
	}

	setCurrentLevel(pContext.getApiLevel());
}


void MsgHandlerApiLevel::setError(const QLatin1String pError)
{
	mJsonObject[QLatin1String("error")] = pError;
}


void MsgHandlerApiLevel::setCurrentLevel(MsgLevel pLevel)
{
	mJsonObject[QLatin1String("current")] = static_cast<int>(pLevel);
}


void MsgHandlerApiLevel::setAvailableLevel()
{
	QJsonArray availableApiLevel;
	const auto& list = Enum<MsgLevel>::getList();
	for (auto entry : list)
	{
		availableApiLevel += static_cast<int>(entry);
	}
	mJsonObject[QLatin1String("available")] = availableApiLevel;
}
