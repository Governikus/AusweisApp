/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApduBuilder.h"

class test_Commands;

namespace governikus
{

class PSOBuilder
	: public CommandApduBuilder
{
	public:
		enum class P1 : int
		{
			DECRYPT = 0x80, ENCRYPT = 0x86, SIGN_HASH = 0x9e, VERIFY = 0x00,
		};

		enum class P2 : int
		{
			UNCRYPTED_DATA = 0x80, ENCRYPTED_DATA = 0x86, HASH_VALUE = 0x9a, CERTIFICATE = 0xbe,
		};

		explicit PSOBuilder(P1 p1, P2 p2);
		void setCertificateBody(const QByteArray& pData);
		void setSignature(const QByteArray& pData);
		CommandApdu build() override;

	private:
		friend class ::test_Commands;
		P1 mP1;
		P2 mP2;
		QByteArray mCertificateBody;
		QByteArray mSignature;
};

} // namespace governikus
