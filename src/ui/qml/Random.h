/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */


#pragma once

#include <QObject>

namespace governikus
{

class Random
	: public QObject
{
	Q_OBJECT

	public:
		static Random& getInstance();

		/// A discrete random distribution on the range [pLowerBound, pUpperBound] with equal probability throughout the range.
		Q_INVOKABLE int randomInt(int pLowerBound, int pUpperBound) const;
};

} // namespace governikus
