/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApduBuilder.h"

class test_Commands;

namespace governikus
{

class GABuilder
	: public CommandApduBuilder
{
	public:
		explicit GABuilder(char pClassByte = CommandApdu::CLA);
		void setCaEphemeralPublicKey(const QByteArray& pData);
		void setPaceMappingData(const QByteArray& pData);
		void setPaceEphemeralPublicKey(const QByteArray& pData);
		void setPaceAuthenticationToken(const QByteArray& pData);
		CommandApdu build() override;

	private:
		friend class ::test_Commands;
		char mClassByte;
		QByteArray mCaEphemeralPublicKey;
		QByteArray mPaceMappingData;
		QByteArray mPaceEphemeralPublicKey;
		QByteArray mPaceAuthenticationToken;
};

} // namespace governikus
