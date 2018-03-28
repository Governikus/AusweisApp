/*!
 * \brief Holds the data of an InputAPDUInfo element.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "Apdu.h"

#include <QByteArrayList>
#include <QList>

#pragma once

namespace governikus
{

class InputAPDUInfo
{
	public:
		InputAPDUInfo();
		InputAPDUInfo(const QByteArray& pInputApdu, bool pUpdateRetryCounter = false);


		bool isValid() const
		{
			return !mInputApdu.isEmpty();
		}


		const CommandApdu getInputApdu() const
		{
			return CommandApdu(mInputApdu, mUpdateRetryCounter);
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
		// mUpdateRetryCounter is not part of the xml data.
		// We use it internally to update the retry counter on a
		// low level especially when we act as a remote card reader
		bool mUpdateRetryCounter;
};

}
