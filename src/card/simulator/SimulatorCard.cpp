/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "SimulatorCard.h"

#include "FileRef.h"
#include "Randomizer.h"
#include "VolatileSettings.h"
#include "apdu/FileCommand.h"
#include "apdu/GeneralAuthenticateResponse.h"
#include "asn1/ASN1Struct.h"
#include "asn1/ASN1TemplateUtil.h"
#include "asn1/ASN1Util.h"
#include "pace/CipherMac.h"
#include "pace/KeyDerivationFunction.h"
#include "pace/ec/EcUtil.h"
#include "pace/ec/EcdhGenericMapping.h"
#include "pace/ec/EcdhKeyAgreement.h"

#include <QCryptographicHash>
#include <QScopeGuard>
#include <QThread>
#include <QtEndian>
#if OPENSSL_VERSION_NUMBER < 0x30000000L
	#include <openssl/bn.h>
#endif


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_simulator)
Q_DECLARE_LOGGING_CATEGORY(secure)


SimulatorCard::SimulatorCard(const SimulatorFileSystem& pFileSystem)
	: Card()
	, mConnected(false)
	, mFileSystem(pFileSystem)
	, mSecureMessaging()
	, mNewSecureMessaging()
	, mSelectedProtocol()
	, mChainingStep(0)
	, mPaceKeyId(0)
	, mPaceChat()
	, mPaceNonce()
	, mPaceTerminalKey()
	, mCardKey()
	, mTaAuxData()
{
}


CardReturnCode governikus::SimulatorCard::establishConnection()
{
	if (!mConnected)
	{
		mConnected = true;
		return CardReturnCode::OK;
	}

	return CardReturnCode::COMMAND_FAILED;
}


CardReturnCode SimulatorCard::releaseConnection()
{
	if (mConnected)
	{
		mConnected = false;
		return CardReturnCode::OK;
	}
	return CardReturnCode::COMMAND_FAILED;
}


bool SimulatorCard::isConnected() const
{
	return mConnected;
}


ResponseApduResult SimulatorCard::transmit(const CommandApdu& pCmd)
{
	CommandApdu commandApdu = pCmd;
	ResponseApduResult result = {CardReturnCode::OK, ResponseApdu(StatusCode::SUCCESS)};

	qCDebug(card_simulator) << "Transmit command APDU:" << commandApdu;

	if (mSecureMessaging)
	{
		if (commandApdu.isSecureMessaging())
		{
			commandApdu = mSecureMessaging->decrypt(commandApdu);
			qCDebug(secure) << "Unencrypted transmit command APDU:" << commandApdu;
		}
		else
		{
			qCDebug(card_simulator) << "Received unencrypted command APDU. Disable secure messaging";
			mSecureMessaging.reset();
			mPaceChat.reset();
		}
	}

	switch (commandApdu.getINS())
	{
		case Ins::SELECT:
		case Ins::READ_BINARY:
		case Ins::UPDATE_BINARY:
			result = executeFileCommand(commandApdu);
			break;

		case Ins::GET_CHALLENGE:
			result = {CardReturnCode::OK, ResponseApdu(QByteArray::fromHex("01020304050607089000"))};
			break;

		case Ins::MSE_SET:
			if (pCmd.getP2() == CommandApdu::AUTHENTICATION_TEMPLATE)
			{
				result = executeMseSetAt(commandApdu);
			}
			break;

		case Ins::GENERAL_AUTHENTICATE:
			result = executeGeneralAuthenticate(commandApdu);
			break;

		case Ins::VERIFY:
			if (commandApdu.isProprietary())
			{
				result = {CardReturnCode::OK, ResponseApdu(verifyAuxiliaryData(commandApdu.getData()))};
				break;
			}

			result = {CardReturnCode::OK, ResponseApdu(StatusCode::UNSUPPORTED_CLA)};
			break;

		default:
			break;
	}

	if (commandApdu.isCommandChaining())
	{
		mChainingStep++;
	}
	else
	{
		mChainingStep = 0;
	}

	if (mSecureMessaging)
	{
		result.mResponseApdu = mSecureMessaging->encrypt(result.mResponseApdu);
	}

	qCDebug(card_simulator) << "Transmit response APDU:" << result.mResponseApdu;

	if (mNewSecureMessaging)
	{
		qCDebug(card_simulator) << "Start to use a new secure messaging channel";
		mNewSecureMessaging.swap(mSecureMessaging);
		mNewSecureMessaging.reset();
	}

	return result;
}


EstablishPaceChannelOutput SimulatorCard::establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription)
{
	Q_UNUSED(pPasswordId)
	Q_UNUSED(pPreferredPinLength)
	Q_UNUSED(pCertificateDescription)

	QThread::msleep(Env::getSingleton<VolatileSettings>()->getDelay());
	EstablishPaceChannelOutput output(CardReturnCode::OK);
	output.setPaceReturnCode(CardReturnCode::OK);
	output.setStatusMseSetAt(QByteArray::fromHex("9000"));
	output.setEfCardAccess(mFileSystem.getEfCardAccess());
	if (!pChat.isEmpty() && !pCertificateDescription.isEmpty())
	{
		mPaceChat = CHAT::decode(pChat);
		output.setCarCurr(QByteArray("DETESTeID00005"));
		output.setIdIcc(QByteArray::fromHex("0102030405060708900A0B0C0D0E0F1011121314"));
	}
	return output;
}


CardReturnCode SimulatorCard::destroyPaceChannel()
{
	mSecureMessaging.reset();
	mPaceChat.reset();

	return CardReturnCode::OK;
}


ResponseApduResult SimulatorCard::setEidPin(quint8 pTimeoutSeconds)
{
	Q_UNUSED(pTimeoutSeconds)

	QThread::msleep(Env::getSingleton<VolatileSettings>()->getDelay());
	return {CardReturnCode::OK, ResponseApdu(QByteArray::fromHex("9000"))};
}


ResponseApduResult SimulatorCard::executeFileCommand(const CommandApdu& pCmd)
{
	const FileCommand fileCommand(pCmd);
	const auto& offset = fileCommand.getOffset();
	const auto& fileRef = fileCommand.getFileRef();
	ResponseApduResult result = {CardReturnCode::OK, ResponseApdu(StatusCode::SUCCESS)};

	switch (pCmd.getINS())
	{
		case Ins::SELECT:
			if (fileRef.getType() == FileRef::ELEMENTARY_FILE)
			{
				const auto& fileId = fileRef.getIdentifier();
				result = {CardReturnCode::OK, ResponseApdu(mFileSystem.select(fileId))};
			}
			break;

		case Ins::READ_BINARY:
			if (offset == 0 && fileRef.getType() == FileRef::TYPE::ELEMENTARY_FILE)
			{
				const auto& selectResult = mFileSystem.select(fileRef.getShortIdentifier());
				result = {CardReturnCode::OK, ResponseApdu(selectResult)};
			}

			if (result.mResponseApdu.getStatusCode() == StatusCode::SUCCESS)
			{
				const auto& file = mFileSystem.read(offset, fileCommand.getLe(), pCmd.isExtendedLength());
				result = {CardReturnCode::OK, ResponseApdu(file)};
			}
			break;

		case Ins::UPDATE_BINARY:
			if (offset == 0 && fileRef.getType() == FileRef::TYPE::ELEMENTARY_FILE)
			{
				const auto& selectResult = mFileSystem.select(fileRef.getShortIdentifier());
				result = {CardReturnCode::OK, ResponseApdu(selectResult)};
			}

			if (result.mResponseApdu.getStatusCode() == StatusCode::SUCCESS)
			{
				const auto& statusCode = mFileSystem.write(offset, pCmd.getData());
				result = {CardReturnCode::OK, ResponseApdu(statusCode)};
			}
			break;

		default:
			result = {CardReturnCode::COMMAND_FAILED};
	}

	return result;
}


ResponseApduResult SimulatorCard::executeMseSetAt(const CommandApdu& pCmd)
{

	if (pCmd.getData().isEmpty())
	{
		return {CardReturnCode::OK, ResponseApdu(StatusCode::WRONG_LENGTH)};
	}

	const ASN1Struct cmdData(pCmd.getData());

	mSelectedProtocol = Oid(cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::CRYPTOGRAPHIC_MECHANISM_REFERENCE));

	const SecurityProtocol protocol(mSelectedProtocol);

	switch (pCmd.getP1())
	{
		case CommandApdu::PACE:
			if (protocol.getProtocol() == ProtocolType::PACE)
			{
				mPaceKeyId = cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::PRIVATE_KEY_REFERENCE).back();
				const auto& chat = cmdData.getObject(V_ASN1_APPLICATION, ASN1Struct::CERTIFICATE_HOLDER_AUTHORIZATION_TEMPLATE);
				if (!chat.isEmpty())
				{
					mPaceChat = CHAT::decode(chat);
				}
				else
				{
					mPaceChat.reset();
				}
			}
			break;

		case CommandApdu::CHIP_AUTHENTICATION:
			if (protocol.getProtocol() == ProtocolType::CA || protocol.getProtocol() == ProtocolType::RI)
			{
				mCardKey = mFileSystem.getKey(cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::PRIVATE_KEY_REFERENCE).back());
			}
			break;

		case CommandApdu::TERMINAL_AUTHENTICATION:
			if (protocol.getProtocol() == ProtocolType::TA)
			{
				const auto& aad = cmdData.getObject(V_ASN1_APPLICATION, ASN1Struct::AUXILIARY_AUTHENTICATED_DATA);
				mTaAuxData = AuthenticatedAuxiliaryData::decode(aad);
			}
			break;

		default:
			return {CardReturnCode::OK, ResponseApdu(StatusCode::INVALID_PARAMETER)};
	}

	return {CardReturnCode::OK, ResponseApdu(StatusCode::SUCCESS)};
}


ResponseApduResult SimulatorCard::executeGeneralAuthenticate(const CommandApdu& pCmd)
{
	if (pCmd.getData().isEmpty())
	{
		return {CardReturnCode::OK, ResponseApdu(StatusCode::WRONG_LENGTH)};
	}

	const ASN1Struct cmdData(pCmd.getData());

	if (mSelectedProtocol == KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128)
	{
		const SecurityProtocol protocol(mSelectedProtocol);
		QByteArray responseData;

		if (mChainingStep <= 2 && !pCmd.isCommandChaining())
		{
			return {CardReturnCode::OK, ResponseApdu(StatusCode::UNSUPPORTED_CLA)};
		}

		switch (mChainingStep)
		{
			case 0:
			{
				mPaceNonce = Randomizer::getInstance().createUuid().toRfc4122();
				const auto& symmetricKey = KeyDerivationFunction(protocol).pi(QByteArray("123456"));
				SymmetricCipher nonceDecrypter(protocol, symmetricKey);
				const auto& encryptedNonce = nonceDecrypter.encrypt(mPaceNonce);

				auto asn1Nonce = newObject<GA_ENCRYPTEDNONCEDATA>();
				Asn1OctetStringUtil::setValue(encryptedNonce, asn1Nonce->mEncryptedNonce);
				responseData = encodeObject(asn1Nonce.data());
				break;
			}

			case 1:
			{
				EcdhGenericMapping mapping(EcUtil::createCurve(PaceInfo::getMappedNid(mPaceKeyId)));
				const auto& localMappingData = mapping.generateLocalMappingData();
				const auto& remoteMappingData = cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::MAPPING_DATA);
				mapping.generateEphemeralDomainParameters(remoteMappingData, mPaceNonce);
				mCardKey = EcUtil::generateKey(mapping.getCurve());
				mPaceNonce.clear();

				auto asn1Mapping = newObject<GA_MAPNONCEDATA>();
				Asn1OctetStringUtil::setValue(localMappingData, asn1Mapping->mMappingData);
				responseData = encodeObject(asn1Mapping.data());
				break;
			}

			case 2:
			{
				mPaceTerminalKey = cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::PACE_EPHEMERAL_PUBLIC_KEY);

				auto asn1KeyAgreement = newObject<GA_PERFORMKEYAGREEMENTDATA>();
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
				const auto& encodedPublicKey = EcUtil::getEncodedPublicKey(mCardKey);
#else
				const auto& curve = EcUtil::create(EC_GROUP_dup(EC_KEY_get0_group(mCardKey.data())));
				const auto& encodedPublicKey = EcUtil::point2oct(curve, EC_KEY_get0_public_key(mCardKey.data()));
#endif
				Asn1OctetStringUtil::setValue(encodedPublicKey, asn1KeyAgreement->mEphemeralPublicKey);
				responseData = encodeObject(asn1KeyAgreement.data());
				break;
			}

			case 3:
			{
				if (pCmd.isCommandChaining())
				{
					mChainingStep = -1;
					return {CardReturnCode::OK, ResponseApdu(StatusCode::LAST_CHAIN_CMD_EXPECTED)};
				}

				const auto& mutualAuthenticationTerminalData = generateAuthenticationToken(mPaceTerminalKey);
				mCardKey.reset();
				mPaceTerminalKey.clear();

				auto ga = newObject<GA_MUTUALAUTHENTICATIONDATA>();
				Asn1OctetStringUtil::setValue(mutualAuthenticationTerminalData, ga->mAuthenticationToken);
				if (mPaceChat)
				{
					ga->mCarCurr = ASN1_OCTET_STRING_new();
					Asn1OctetStringUtil::setValue(QByteArray("DETESTeID00005"), ga->mCarCurr);
				}
				responseData = encodeObject(ga.data());
				break;
			}

			default:
				Q_UNREACHABLE();
		}

		return {CardReturnCode::OK, ResponseApdu(responseData + QByteArray::fromHex("9000"))};
	}

	if (mSelectedProtocol == KnownOid::ID_CA_ECDH_AES_CBC_CMAC_128)
	{
		const auto& caKey = cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::CA_EPHEMERAL_PUBLIC_KEY);
		if (caKey.isEmpty())
		{
			return {CardReturnCode::OK, ResponseApdu(StatusCode::INVALID_DATAFIELD)};
		}

		const QByteArray nonce = QByteArray::fromHex("0001020304050607");
		const QByteArray& authenticationToken = generateAuthenticationToken(caKey, nonce);

		auto ga = newObject<GA_CHIPAUTHENTICATIONDATA>();
		Asn1OctetStringUtil::setValue(nonce, ga->mNonce);
		Asn1OctetStringUtil::setValue(authenticationToken, ga->mAuthenticationToken);
		return {CardReturnCode::OK, ResponseApdu(encodeObject(ga.data()) + QByteArray::fromHex("9000"))};
	}

	if (mSelectedProtocol == KnownOid::ID_RI_ECDH_SHA_256)
	{
		const auto& rawOid = cmdData.getData(V_ASN1_UNIVERSAL, ASN1Struct::UNI_OBJECT_IDENTIFIER);
		const auto& riKey = cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::EC_PUBLIC_POINT);
		if (rawOid.isEmpty() || riKey.isEmpty())
		{
			return {CardReturnCode::OK, ResponseApdu(StatusCode::INVALID_DATAFIELD)};
		}

		const Oid oid(rawOid);
		if (oid != KnownOid::ID_RI_ECDH_SHA_256)
		{
			return {CardReturnCode::OK, ResponseApdu(StatusCode::NO_BINARY_FILE)};
		}

		const QByteArray& restrictedId = generateRestrictedId(riKey);

		const auto responseData = Asn1Util::encode(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::RI_FIRST_IDENTIFIER, restrictedId);
		const auto response = Asn1Util::encode(V_ASN1_APPLICATION, ASN1Struct::DYNAMIC_AUTHENTICATION_DATA, responseData, true);
		return {CardReturnCode::OK, ResponseApdu(response + QByteArray::fromHex("9000"))};
	}

	return {CardReturnCode::OK, ResponseApdu(StatusCode::SUCCESS)};
}


QByteArray SimulatorCard::ecMultiplication(const QByteArray& pPoint) const
{
	if (mCardKey.isNull())
	{
		qCCritical(card_simulator) << "Missing private key";
		return QByteArray();
	}

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	const auto& terminalKey = EcUtil::create(EVP_PKEY_new());
	if (terminalKey.isNull() || EVP_PKEY_copy_parameters(terminalKey.data(), mCardKey.data()) == 0)
	{
		qCCritical(card_simulator) << "Initialization of the terminal key failed";
		return QByteArray();
	}
	if (!EVP_PKEY_set1_encoded_public_key(
			terminalKey.data(),
			reinterpret_cast<const unsigned char*>(pPoint.data()),
			static_cast<size_t>(pPoint.length())))
	{
		qCCritical(card_simulator) << "Interpreting the terminal key failed";
		return QByteArray();
	}

	const auto& ctx = EcUtil::create(EVP_PKEY_CTX_new_from_pkey(nullptr, mCardKey.data(), nullptr));
	size_t resultLen = 0;
	if (EVP_PKEY_derive_init(ctx.data()) <= 0
			|| EVP_PKEY_derive_set_peer(ctx.data(), terminalKey.data()) <= 0
			|| EVP_PKEY_derive(ctx.data(), nullptr, &resultLen) <= 0)
	{
		qCCritical(card_simulator) << "Initialization or calculation of the result failed";
		return QByteArray();
	}

	QByteArray result(static_cast<qsizetype>(resultLen), '\0');
	if (EVP_PKEY_derive(ctx.data(), reinterpret_cast<uchar*>(result.data()), &resultLen) <= 0)
	{
		qCCritical(card_simulator) << "Calculation of the result failed";
		return QByteArray();
	}

	return result;

#else
	const auto& curve = EcUtil::create(EC_GROUP_dup(EC_KEY_get0_group(mCardKey.data())));
	auto point = EcUtil::oct2point(curve, pPoint);
	if (!point)
	{
		qCCritical(card_simulator) << "Interpreting the point failed";
		return QByteArray();
	}

	QSharedPointer<EC_POINT> result = EcUtil::create(EC_POINT_new(curve.data()));
	const auto& privateKey = EcUtil::create(BN_dup(EC_KEY_get0_private_key(mCardKey.data())));
	if (!EC_POINT_mul(curve.data(), result.data(), nullptr, point.data(), privateKey.data(), nullptr))
	{
		qCCritical(card_simulator) << "Calculation of the result failed";
		return QByteArray();
	}

	return EcUtil::point2oct(curve, result.data(), true);

#endif
}


QByteArray SimulatorCard::generateAuthenticationToken(const QByteArray& pPublicKey, const QByteArray& pNonce)
{
	QByteArray sharedSecret = ecMultiplication(pPublicKey);

	const auto protocol = SecurityProtocol(mSelectedProtocol);
	KeyDerivationFunction kdf(protocol);

	QByteArray macKey = kdf.mac(sharedSecret, pNonce);
	QByteArray encKey = kdf.enc(sharedSecret, pNonce);

	mNewSecureMessaging.reset(new SecureMessaging(protocol, encKey, macKey));

	CipherMac cmac(protocol, macKey);
	return cmac.generate(EcdhKeyAgreement::encodeUncompressedPublicKey(mSelectedProtocol, pPublicKey));
}


QByteArray SimulatorCard::generateRestrictedId(const QByteArray& pPublicKey) const
{
	QByteArray sharedSecret = ecMultiplication(pPublicKey);

	return QCryptographicHash::hash(sharedSecret, QCryptographicHash::Sha256);
}


StatusCode SimulatorCard::verifyAuxiliaryData(const QByteArray& pASN1Struct)
{
	const auto* unsignedCharPointer = reinterpret_cast<const uchar*>(pASN1Struct.constData());
	ASN1_OBJECT* obj = d2i_ASN1_OBJECT(nullptr, &unsignedCharPointer, static_cast<long>(pASN1Struct.size()));
	const auto guard = qScopeGuard([obj] {ASN1_OBJECT_free(obj);});
	return mFileSystem.verify(Oid(obj), mTaAuxData);
}
