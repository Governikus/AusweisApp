/*
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "LanguageLoader.h"

#include <QJsonValue>
#include <QMap>
#include <QString>


namespace governikus
{

class LanguageString
{
	friend inline bool operator==(const LanguageString& pLeft, const LanguageString& pRight);

	private:
		QMap<QString, QString> mStrings;

		QString toString(const QLocale& pLocale) const;

	public:
		LanguageString(const QJsonValue& pJson);
		LanguageString(const QMap<QString, QString>& pInput);
		LanguageString(const QString& pString, const QLocale& pLocale = LanguageLoader::getInstance().getUsedLocale());


		bool isEmpty() const;
		QString toString() const;
		operator QString() const;

		QMap<QString, QString>::const_iterator begin() const;
		QMap<QString, QString>::const_iterator end() const;
};

inline bool operator==(const LanguageString& pLeft, const LanguageString& pRight)
{
	return pLeft.mStrings == pRight.mStrings;
}


} /* namespace governikus */
