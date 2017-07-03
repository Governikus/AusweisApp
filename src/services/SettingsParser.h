/*!
 * SettingsParser.h
 *
 * \brief Abstract definition of a parser that returns value of some class R.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once


#include <QByteArray>
#include <QSharedPointer>

namespace governikus
{
template<class R>
class SettingsParser
{
	protected:
		SettingsParser()
		{
		}


	public:
		virtual ~SettingsParser()
		{
		}


		/*!
		 * Parses the data and returns a value wrapped in a QSharedPointer.
		 *
		 * In case of any errors, the QSharedPointer is empty.
		 */
		virtual QSharedPointer<R> parse(const QByteArray& pData) = 0;
};


} /* namespace governikus */
