/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
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
	if (const auto& jsonLevel = pObj[QLatin1String("level")]; jsonLevel.isUndefined())
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
#if (QT_VERSION >= QT_VERSION_CHECK(6, 9, 0))
		if (Enum<MsgLevel>::isValue(static_cast<quint64>(level)))
#else
		if (Enum<MsgLevel>::isValue(level))
#endif
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
	setValue(QLatin1String("error"), pError);
}


void MsgHandlerApiLevel::setCurrentLevel(MsgLevel pLevel)
{
	setValue(QLatin1String("current"), static_cast<int>(pLevel));
}


void MsgHandlerApiLevel::setAvailableLevel()
{
	QJsonArray availableApiLevel;
	const auto& list = Enum<MsgLevel>::getList();
	for (auto entry : list)
	{
		availableApiLevel += static_cast<int>(entry);
	}
	setValue(QLatin1String("available"), availableApiLevel);
}
