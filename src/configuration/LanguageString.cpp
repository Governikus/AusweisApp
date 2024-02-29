/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "LanguageString.h"

#include <QJsonArray>
#include <QJsonObject>

using namespace governikus;


LanguageString::LanguageString(const QJsonValue& pJson)
{
	if (!pJson.isObject())
	{
		return;
	}

	const QJsonObject& object = pJson.toObject();
	const auto& iterEnd = object.constEnd();
	for (auto iter = object.constBegin(); iter != iterEnd; ++iter)
	{
		mStrings[iter.key()] = iter.value().toString();
	}
}


LanguageString::LanguageString(const QMap<QString, QString>& pInput)
	: mStrings(pInput)
{
}


LanguageString::LanguageString(const QString& pString, const QLocale& pLocale)
{
	mStrings[pLocale.name()] = pString;
}


QString LanguageString::toString() const
{
	QString result = toString(LanguageLoader::getInstance().getUsedLocale());
	if (!result.isNull())
	{
		return result;
	}

	result = toString(LanguageLoader::getInstance().getFallbackLanguage());
	if (!result.isNull())
	{
		return result;
	}

	return mStrings[QString()];
}


QString LanguageString::toString(const QLocale& pLocale) const
{
	QString result = mStrings[pLocale.name()];
	if (!result.isNull())
	{
		return result;
	}

	result = mStrings[LanguageLoader::getLocaleCode(pLocale)];
	if (!result.isNull())
	{
		return result;
	}

	return QString();
}


bool LanguageString::isEmpty() const
{
	return mStrings.isEmpty();
}


QMap<QString, QString>::const_iterator LanguageString::begin() const
{
	return mStrings.begin();
}


QMap<QString, QString>::const_iterator LanguageString::end() const
{
	return mStrings.end();
}


LanguageString::operator QString() const
{
	return toString();
}
