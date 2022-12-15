/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

#include <QByteArray>
#include <QDebug>

#include <cstddef>


namespace governikus
{
defineTypedEnumType(Ins, uchar,
		UNKNOWN = 0x00,
		DEACTIVATE = 0x04,
		VERIFY = 0x20,
		MSE_SET = 0x22,
		ACTIVATE = 0x44,
		EXTERNAL_AUTHENTICATE = 0x82,
		GET_CHALLENGE = 0x84,
		GENERAL_AUTHENTICATE = 0x86,
		PSO_VERIFY = 0x2A,
		PSO_COMPUTE = 0x2B,
		RESET_RETRY_COUNTER = 0x2C,
		SELECT = 0xA4,
		READ_BINARY = 0xB0,
		GET_RESPONSE = 0xC0,
		UPDATE_BINARY = 0xD6
		)

class CommandApdu
{
	private:
		std::byte mCla;
		uchar mIns;
		uchar mP1;
		uchar mP2;
		QByteArray mData;
		int mLe;

	public:
		enum Param : uchar
		{
			IMPLICIT = 0x00,
			CHANGE = 0x02,
			UNBLOCK = 0x03,
			PIN = 0x03,
			CHIP_AUTHENTICATION = 0x41,
			VERIFICATION = 0x81,
			AUTHENTICATION_TEMPLATE = 0xA4,
			DIGITAL_SIGNATURE_TEMPLATE = 0xB6,
			SELF_DESCRIPTIVE = 0xBE,
			PACE = 0xC1
		};

		static const int NO_LE = 0;
		static const int SHORT_MAX_LC = 0xFF;
		static const int SHORT_MAX_LE = 0x0100;
		static const int EXTENDED_MAX_LC = 0x00FFFF;
		static const int EXTENDED_MAX_LE = 0x010000;

		[[nodiscard]] static bool isExtendedLength(const QByteArray& pData, int pLe);

		explicit CommandApdu(const QByteArray& pBuffer = QByteArray());
		explicit CommandApdu(const QByteArray& pHeader, const QByteArray& pData, int pLe = NO_LE);
		explicit CommandApdu(Ins pIns, uchar pP1, uchar pP2, const QByteArray& pData = QByteArray(), int pLe = NO_LE);
		virtual ~CommandApdu();

		[[nodiscard]] bool isProprietary() const;

		void enableCommandChaining();
		[[nodiscard]] bool isCommandChaining() const;

		void setSecureMessaging(bool pEnabled);
		[[nodiscard]] bool isSecureMessaging() const;

		[[nodiscard]] bool isEmpty() const;
		[[nodiscard]] Ins getINS() const;
		[[nodiscard]] uchar getP1() const;
		[[nodiscard]] uchar getP2() const;
		[[nodiscard]] QByteArray getHeaderBytes() const;
		[[nodiscard]] const QByteArray& getData() const;
		[[nodiscard]] int getLe() const;
		[[nodiscard]] bool isExtendedLength() const;

		operator QByteArray() const;
};


inline QDebug operator<<(QDebug pDbg, const CommandApdu& pCommandApdu)
{
	QDebugStateSaver saver(pDbg);
	pDbg << QByteArray(pCommandApdu).toHex();
	return pDbg;
}


char* toString(const CommandApdu& pCommandApdu);

#ifndef QT_NO_DEBUG
inline bool operator==(const CommandApdu& pLeft, const CommandApdu& pRight)
{
	return QByteArray(pLeft) == QByteArray(pRight);
}


#endif


} // namespace governikus
