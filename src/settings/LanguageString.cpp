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
	for (const QString& key : object.keys())
	{
		mStrings[key] = object[key].toString();
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
	if (!result.isEmpty())
	{
		return result;
	}

	result = toString(LanguageLoader::getInstance().getFallbackLanguage());
	if (!result.isEmpty())
	{
		return result;
	}

	return mStrings[QString()];
}


QString LanguageString::toString(const QLocale& pLocale) const
{
	QString result = mStrings[pLocale.name()];
	if (!result.isEmpty())
	{
		return result;
	}

	result = mStrings[pLocale.bcp47Name()];
	if (!result.isEmpty())
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
