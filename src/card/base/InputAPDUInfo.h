/*!
 * \brief Holds the data of an InputAPDUInfo element.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "CommandApdu.h"

#include <QByteArrayList>
#include <QList>

#pragma once

namespace governikus
{

class InputAPDUInfo
{
	public:
		InputAPDUInfo();
		InputAPDUInfo(const QByteArray& pInputApdu);


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
