/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Apdu.h"

#include <QByteArray>

namespace governikus
{

class CommandApdu final
	: public Apdu
{
	private:
		[[nodiscard]] inline bool isExtendedLength() const;

	public:
		static const int NO_LE = 0;
		static const int SHORT_MAX_LC = 0xff;
		static const int SHORT_MAX_LE = 0x0100;
		static const int EXTENDED_MAX_LC = 0x00ffff;
		static const int EXTENDED_MAX_LE = 0x010000;
		static const char CLA = 0x00;
		static const char CLA_COMMAND_CHAINING = 0x10;
		static const char CLA_SECURE_MESSAGING = 0x0c;

		explicit CommandApdu(const QByteArray& pBuffer);
		explicit CommandApdu(const QByteArray& pHeader, const QByteArray& pData, int pLe);
		explicit CommandApdu(char pCla, char pIns, char pP1, char pP2, const QByteArray& pData = QByteArray(), int pLe = NO_LE);

		[[nodiscard]] char getCLA() const;
		[[nodiscard]] char getINS() const;
		[[nodiscard]] char getP1() const;
		[[nodiscard]] char getP2() const;
		[[nodiscard]] int getLc() const;
		[[nodiscard]] int getLe() const;
		[[nodiscard]] QByteArray getData() const;

		static bool isExtendedLength(const QByteArray& pData, int pLe);
		static bool isSecureMessaging(const QByteArray& pCommandBuffer);
};

} // namespace governikus
