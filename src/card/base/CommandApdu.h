/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Apdu.h"

namespace governikus
{

class CommandApdu final
	: public Apdu
{
	private:
		inline bool isExtendedLength() const;

	public:
		static const int NO_LE = 0;
		static const int SHORT_MAX_LC = 0xff;
		static const int SHORT_MAX_LE = 0x0100;
		static const int EXTENDED_MAX_LC = 0x00ffff;
		static const int EXTENDED_MAX_LE = 0x010000;
		static const char CLA = 0x00;
		static const char CLA_COMMAND_CHAINING = 0x10;
		static const char CLA_SECURE_MESSAGING = 0x0c;

		CommandApdu(const QByteArray& pBuffer);
		CommandApdu(const QByteArray& pHeader, const QByteArray& pData, int pLe);
		CommandApdu(char pCla, char pIns, char pP1, char pP2, const QByteArray& pData = QByteArray(), int pLe = NO_LE);

		char getCLA() const;
		char getINS() const;
		char getP1() const;
		char getP2() const;
		int getLc() const;
		int getLe() const;
		QByteArray getData() const;

		static bool isExtendedLength(const QByteArray& pData, int pLe);
		static bool isSecureMessaging(const QByteArray& pCommandBuffer);
};

} // namespace governikus
