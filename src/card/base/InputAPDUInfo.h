/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "apdu/CommandApdu.h"

#include <QByteArrayList>


namespace governikus
{

class InputAPDUInfo
{
	public:
		explicit InputAPDUInfo(const QByteArray& pInputApdu = QByteArray());


		[[nodiscard]] bool isValid() const
		{
			return !mInputApdu.isEmpty();
		}


		[[nodiscard]] CommandApdu getInputApdu() const
		{
			return CommandApdu(mInputApdu);
		}


		void setInputApdu(const QByteArray& pInputApdu)
		{
			mInputApdu = pInputApdu;
		}


		[[nodiscard]] const QByteArrayList& getAcceptableStatusCodes() const
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
