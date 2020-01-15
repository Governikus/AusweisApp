/*!
 * \brief Holds the data of an InputAPDUInfo element.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApdu.h"

#include <QByteArrayList>

namespace governikus
{

class InputAPDUInfo
{
	public:
		explicit InputAPDUInfo(const QByteArray& pInputApdu = QByteArray());


		bool isValid() const
		{
			return !mInputApdu.isEmpty();
		}


		const CommandApdu getInputApdu() const
		{
			return CommandApdu(mInputApdu);
		}


		void setInputApdu(const QByteArray& pInputApdu)
		{
			mInputApdu = pInputApdu;
		}


		const QByteArrayList& getAcceptableStatusCodes() const
		{
			return mAcceptableStatusCodes;
		}


		void addAcceptableStatusCode(const QByteArray& pStatusCodeAsHex)
		{
			mAcceptableStatusCodes += pStatusCodeAsHex;
		}


	private:
		QByteArray mInputApdu;
		QByteArrayList mAcceptableStatusCodes;
};

} // namespace governikus
