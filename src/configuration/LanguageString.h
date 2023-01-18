/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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

		[[nodiscard]] QString toString(const QLocale& pLocale) const;

	public:
		explicit LanguageString(const QJsonValue& pJson);
		explicit LanguageString(const QMap<QString, QString>& pInput);
		LanguageString(const QString& pString, const QLocale& pLocale = LanguageLoader::getInstance().getUsedLocale());


		[[nodiscard]] bool isEmpty() const;
		[[nodiscard]] QString toString() const;
		operator QString() const;

		[[nodiscard]] QMap<QString, QString>::const_iterator begin() const;
		[[nodiscard]] QMap<QString, QString>::const_iterator end() const;
};

inline bool operator==(const LanguageString& pLeft, const LanguageString& pRight)
{
	return pLeft.mStrings == pRight.mStrings;
}


} // namespace governikus
