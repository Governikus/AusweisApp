/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "Apdu.h"
#include "CardReturnCode.h"
#include "FileRef.h"
#include "SmartCardDefinitions.h"
#include "asn1/Chat.h"

#include <QtEndian>


namespace governikus
{

// TODO: brauchen wir das wirklich?
template<typename T> QByteArray toBigEndian(T pDataToConvert)
{
	uchar converted[sizeof(T)];
	qToBigEndian(pDataToConvert, converted);

	unsigned long position;
	for (position = 0; position < sizeof(T) - 1; ++position)
	{
		if (converted[position] != 0)
		{
			break;
		}
	}

	return QByteArray(reinterpret_cast<char*>(&converted[position]), static_cast<int>(sizeof(T) - position));
}


class CommandApduBuilder
{
	private:
		Q_DISABLE_COPY(CommandApduBuilder)

	public:
		CommandApduBuilder();
		virtual ~CommandApduBuilder();
		virtual CommandApdu build() = 0;
};

class SelectBuilder
	: public CommandApduBuilder
{
	private:
		const FileRef mFileRef;

	public:
		enum class P1 : char
		{
			SELECT_MF = 0x00, CHILD_DF = 0x01, CHILD_EF = 0x02, PARENT_DF = 0x03, APPLICATION_ID = 0x04, ABS_PATH = 0x08, REL_PATH = 0x09,
		};

		enum class P2 : char
		{
			FCI = 0x00, FCP = 0x04, FMD = 0x08, NONE = 0x0c,
		};

		SelectBuilder(const FileRef& pFileRef);
		CommandApdu build() override;
};

class GetChallengeBuilder
	: public CommandApduBuilder
{
	public:
		GetChallengeBuilder();
		CommandApdu build() override;
};

class GetChallengeResponse
	: public ResponseApdu
{
	public:
		GetChallengeResponse();
		virtual ~GetChallengeResponse();
		QByteArray getChallenge() const;
};

class MSEBuilder
	: public CommandApduBuilder
{
	public:
		enum class P1 : int
		{
			COMPUTE_DIGITAL_SIGNATURE = 0x41, PUT_HASH = 0xa0, PERFORM_SECURITY_OPERATION = 0xc1, SET_DST = 0x81, ERASE = 0xF3,
		};

		enum class P2 : int
		{
			SET_AT = 0xa4, HASH_ALGORITHM = 0xaa, COMPUTE_DIGITAL_SIGNATURE = 0xb6, ENCRYPTION_OPERATION = 0xb8, DEFAULT_CHANNEL = 0x00,
		};

		MSEBuilder(P1 p1, P2 p2);
		void setAuxiliaryData(const QByteArray& pData);
		void setOid(const QByteArray& pData);
		void setPublicKey(const QByteArray& pData);
		void setPublicKey(PACE_PIN_ID pPin);
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

		PSOBuilder(P1 p1, P2 p2);
		void setCertificateBody(const QByteArray& pData);
		void setSignature(const QByteArray& pData);
		CommandApdu build() override;

	private:
		P1 mP1;
		P2 mP2;
		QByteArray mCertificateBody;
		QByteArray mSignature;
};

class EABuilder
	: public CommandApduBuilder
{
	public:
		EABuilder();
		void setSignature(const QByteArray& pData);
		CommandApdu build() override;

	private:
		QByteArray mSignature;
};

class GABuilder
	: public CommandApduBuilder
{
	public:
		GABuilder(char pClassByte = CommandApdu::CLA);
		void setCaEphemeralPublicKey(const QByteArray& pData);
		void setPaceMappingData(const QByteArray& pData);
		void setPaceEphemeralPublicKey(const QByteArray& pData);
		void setPaceAuthenticationToken(const QByteArray& pData);
		CommandApdu build() override;

	private:
		char mClassByte;
		QByteArray mCaEphemeralPublicKey;
		QByteArray mPaceMappingData;
		QByteArray mPaceEphemeralPublicKey;
		QByteArray mPaceAuthenticationToken;
};

class ReadBinaryBuilder
	: public CommandApduBuilder
{
	private:
		uint mOffset;
		int mLe;

	public:
		ReadBinaryBuilder(uint pOffset, int pLe);
		CommandApdu build() override;
};

class ResetRetryCounterBuilder
	: public CommandApduBuilder
{
	public:
		ResetRetryCounterBuilder(const QByteArray& pPin = QByteArray());
		CommandApdu build() override;

	private:
		QByteArray mPin;
};


class PinModifyBuilder
{
	private:
		QByteArray createCommandData(quint8 pTimeoutSeconds, char pMsgIndex1, char pMsgIndex2, char pMsgIndex3, const QByteArray& pAbData) const;

	public:
		QByteArray createChangeEidPinCommandData(quint8 pTimeoutSeconds) const;

		/**
		 * According to DWG_Smart-Card_CCID_Rev110.pdf as mentioned in [TR-03110].
		 */
		CommandApdu createCommandDataCcid(quint8 pTimeoutSeconds) const;
};

class PinModifyOutput
{
	private:
		CardReturnCode mReturnCode;

	public:
		void parse(const QByteArray& pData);
		void parseFromCcid(const QByteArray& pData);
		CardReturnCode getReturnCode() const;
};

} /* namespace governikus */
