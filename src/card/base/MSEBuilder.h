/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApduBuilder.h"
#include "SmartCardDefinitions.h"

class test_Commands;

namespace governikus
{

class MSEBuilder
	: public CommandApduBuilder
{
	public:
		enum class INS : char
		{
			MANAGE_SECURITY_ENVIRONMENT = 0x22,
		};

		enum class P1 : char
		{
			COMPUTE_DIGITAL_SIGNATURE = 0x41, PUT_HASH = char(0xa0), PERFORM_SECURITY_OPERATION = char(0xc1), SET_DST = char(0x81), ERASE = char(0xF4),
		};

		enum class P2 : char
		{
			SET_AT = char(0xa4), HASH_ALGORITHM = char(0xaa), COMPUTE_DIGITAL_SIGNATURE = char(0xb6), ENCRYPTION_OPERATION = char(0xb8), DEFAULT_CHANNEL = 0x01,
		};

		explicit MSEBuilder(P1 p1, P2 p2);
		void setAuxiliaryData(const QByteArray& pData);
		void setOid(const QByteArray& pData);
		void setPublicKey(const QByteArray& pData);
		void setPublicKey(PacePasswordId pPassword);
		void setPrivateKey(const QByteArray& pData);
		void setEphemeralPublicKey(const QByteArray& pData);
		void setChat(const QByteArray& pData);
		CommandApdu build() override;

	private:
		P1 mP1;
		P2 mP2;
		QByteArray mAuxiliaryData;
		QByteArray mOid;
		QByteArray mPublicKey;
		QByteArray mPrivateKey;
		QByteArray mEphemeralPublicKey;
		QByteArray mChat;
};

} // namespace governikus
