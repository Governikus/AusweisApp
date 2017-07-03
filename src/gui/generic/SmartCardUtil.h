/*!
 * SmartCardUtil.h
 *
 * \brief Util class to provide icons for Reader and Card.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "SmartCardDefinitions.h"

#include <QString>

namespace governikus
{

class SmartCardUtil
{
	private:
		SmartCardUtil();
		Q_DISABLE_COPY(SmartCardUtil)

	public:
		/*!
		 * Gets the icon path for the specified reader type.
		 * \param pReaderType type of the reader
		 * \return icon path
		 */
		static QString getReaderIconPath(ReaderType pReaderType);

		/*!
		 * Gets the icon path for the specified empty npa reader type.
		 * \param pReaderType type of the reader
		 * \return icon path
		 */
		static QString getReaderEmptyIconPath(ReaderType pReaderType);

		/*!
		 * Gets the icon path for the specified card type.
		 * \param pCardType type of the card
		 * \return icon path
		 */
		static QString getCardIconPath(CardType pCardType);

		/*!
		 * Gets icon for multiple readers
		 * \return icon path
		 */
		static QString getMultipleReaderIconPath();

		/*!
		 * Gets icon for "no reader found"
		 * \return icon path
		 */
		static QString getNoReaderFoundIconPath();
};

} /* namespace governikus */
