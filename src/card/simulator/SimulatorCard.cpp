/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
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
#include "asn1/AuthenticatedAuxiliaryData.h"
#include "asn1/SignatureChecker.h"
#include "pace/CipherMac.h"
#include "pace/KeyDerivationFunction.h"
#include "pace/ec/EcUtil.h"
#include "pace/ec/EcdhGenericMapping.h"
#include "pace/ec/EcdhKeyAgreement.h"

#include <QCryptographicHash>
#include <QScopeGuard>
#include <QThread>
#include <QtEndian>


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
	, mAccessRights()
	, mPacePassword(PacePasswordId::UNKNOWN)
	, mPaceKeyId(0)
	, mPaceNonce()
	, mPaceTerminalKey()
	, mCardKey()
	, mTaCertificate()
	, mTaSigningData()
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
			mTaCertificate.reset();
			mAccessRights.clear();
		}
	}

	ResponseApdu result = executeCommand(commandApdu);
	if (result.isEmpty())
	{
		result = ResponseApdu(StatusCode::NO_PRECISE_DIAGNOSIS);
		Q_ASSERT(false);
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
		result = mSecureMessaging->encrypt(result);
	}

	qCDebug(card_simulator) << "Transmit response APDU:" << result;

	if (mNewSecureMessaging)
	{
		qCDebug(card_simulator) << "Start to use a new secure messaging channel";
		mNewSecureMessaging.swap(mSecureMessaging);
		mNewSecureMessaging.reset();
	}

	return {CardReturnCode::OK, result};
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
	if (pChat.isEmpty())
	{
		mAccessRights.clear();
		mTaCertificate.reset();
		mTaSigningData.clear();
	}
	else
	{
		mAccessRights = CHAT::decode(pChat)->getAccessRights();
		mTaCertificate = mFileSystem.getTrustPoint();

		const int nid = EFCardAccess::decode(mFileSystem.getEfCardAccess())->getPaceInfos().at(0)->getParameterIdAsNid();
		EcdhGenericMapping cardMapping(EcUtil::createCurve(nid));
		EcdhGenericMapping terminalMapping(EcUtil::createCurve(nid));
		const auto& nonce = Randomizer::getInstance().createBytes(16);
		cardMapping.generateLocalMappingData();
		cardMapping.generateEphemeralDomainParameters(terminalMapping.generateLocalMappingData(), nonce);
		mTaSigningData = EcUtil::getEncodedPublicKey(EcUtil::generateKey(cardMapping.getCurve()), true);

		output.setCarCurr(mTaCertificate->getBody().getCertificateHolderReference());
		output.setIdIcc(mTaSigningData);
	}

	return output;
}


CardReturnCode SimulatorCard::destroyPaceChannel()
{
	mSecureMessaging.reset();

	mAccessRights.clear();
	mTaCertificate.reset();
	mTaSigningData.clear();

	return CardReturnCode::OK;
}


ResponseApduResult SimulatorCard::setEidPin(quint8 pTimeoutSeconds)
{
	Q_UNUSED(pTimeoutSeconds)

	QThread::msleep(Env::getSingleton<VolatileSettings>()->getDelay());
	return {CardReturnCode::OK, ResponseApdu(StatusCode::SUCCESS)};
}


ResponseApdu SimulatorCard::executeCommand(const CommandApdu& pCommandApdu)
{
	switch (pCommandApdu.getINS())
	{
		case Ins::SELECT:
		case Ins::READ_BINARY:
		case Ins::UPDATE_BINARY:
			return executeFileCommand(pCommandApdu);

		case Ins::GET_CHALLENGE:
		{
			const auto& challenge = Randomizer::getInstance().createBytes(8);
			mTaSigningData.append(challenge);
			return ResponseApdu(StatusCode::SUCCESS, challenge);
		}

		case Ins::MSE_SET:
			if (pCommandApdu.getP2() == CommandApdu::AUTHENTICATION_TEMPLATE)
			{
				return executeMseSetAt(pCommandApdu);
			}

			if (pCommandApdu.getP2() == CommandApdu::DIGITAL_SIGNATURE_TEMPLATE)
			{
				return executeMseSetDst(pCommandApdu.getData());
			}

			return ResponseApdu(StatusCode::INVALID_PARAMETER);

		case Ins::GENERAL_AUTHENTICATE:
			return executeGeneralAuthenticate(pCommandApdu);

		case Ins::EXTERNAL_AUTHENTICATE:
			return executeExternalAuthenticate(pCommandApdu.getData());

		case Ins::VERIFY:
			if (pCommandApdu.isProprietary())
			{
				return ResponseApdu(verifyAuxiliaryData(pCommandApdu.getData()));
			}

			return ResponseApdu(StatusCode::UNSUPPORTED_CLA);

		case Ins::PSO_VERIFY:
			return executePsoVerify(pCommandApdu.getData());

		case Ins::ACTIVATE:
		case Ins::DEACTIVATE:
			return executePinManagement(pCommandApdu);

		case Ins::RESET_RETRY_COUNTER:
			return executeResetRetryCounter(pCommandApdu);

		default:
			qCCritical(card_simulator) << "Unknown instruction:" << QByteArray(pCommandApdu).toHex();
			return ResponseApdu(StatusCode::UNSUPPORTED_INS);
	}
}


ResponseApdu SimulatorCard::executeFileCommand(const CommandApdu& pCmd)
{
	const FileCommand fileCommand(pCmd);
	const auto& offset = fileCommand.getOffset();
	const auto& fileRef = fileCommand.getFileRef();

	switch (pCmd.getINS())
	{
		case Ins::SELECT:
		{
			const auto& fileId = fileRef.getIdentifier();
			switch (fileRef.getType())
			{
				case FileRef::MASTER_FILE:
					return ResponseApdu(fileId == FileRef::masterFile().getIdentifier() ? StatusCode::SUCCESS : StatusCode::FILE_NOT_FOUND);

				case FileRef::ELEMENTARY_FILE:
					return ResponseApdu(mFileSystem.select(fileId));

				case FileRef::APPLICATION:
					return ResponseApdu(fileId == FileRef::appEId().getIdentifier() ? StatusCode::SUCCESS : StatusCode::FILE_NOT_FOUND);

				default:
					return ResponseApdu(StatusCode::FILE_NOT_FOUND);
			}
		}

		case Ins::READ_BINARY:
			if (offset == 0 && fileRef.getType() == FileRef::TYPE::ELEMENTARY_FILE)
			{
				const auto& selectResult = mFileSystem.select(fileRef.getShortIdentifier());
				if (selectResult != StatusCode::SUCCESS)
				{
					return ResponseApdu(selectResult);
				}
			}

			return ResponseApdu(mFileSystem.read(offset, fileCommand.getLe(), pCmd.isExtendedLength()));

		case Ins::UPDATE_BINARY:
			if (offset == 0 && fileRef.getType() == FileRef::TYPE::ELEMENTARY_FILE)
			{
				const auto& selectResult = mFileSystem.select(fileRef.getShortIdentifier());
				if (selectResult != StatusCode::SUCCESS)
				{
					return ResponseApdu(selectResult);
				}
			}

			return ResponseApdu(mFileSystem.write(offset, pCmd.getData()));

		default:
			return ResponseApdu(StatusCode::UNSUPPORTED_INS);
	}
}


ResponseApdu SimulatorCard::executeMseSetAt(const CommandApdu& pCmd)
{
	if (pCmd.getData().isEmpty())
	{
		return ResponseApdu(StatusCode::WRONG_LENGTH);
	}

	const ASN1Struct cmdData(pCmd.getData());
	mSelectedProtocol = Oid(cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::CRYPTOGRAPHIC_MECHANISM_REFERENCE));
	const SecurityProtocol protocol(mSelectedProtocol);

	switch (pCmd.getP1())
	{
		case CommandApdu::PACE:
		{
			mPacePassword = PacePasswordId(cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::PASSWORD_REFERENCE).back());
			if (protocol.getProtocol() != ProtocolType::PACE || mFileSystem.getPassword(mPacePassword).isNull())
			{
				return ResponseApdu(StatusCode::REFERENCED_DATA_NOT_FOUND);
			}

			mPaceKeyId = cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::PRIVATE_KEY_REFERENCE).back();
			const auto& chat = cmdData.getObject(V_ASN1_APPLICATION, ASN1Struct::CERTIFICATE_HOLDER_AUTHORIZATION_TEMPLATE);
			if (chat.isEmpty())
			{
				mAccessRights.clear();
				mTaCertificate.reset();
			}
			else
			{
				mAccessRights = CHAT::decode(chat)->getAccessRights();
				mTaCertificate = mFileSystem.getTrustPoint();
			}
			mTaSigningData.clear();

			return ResponseApdu(StatusCode::SUCCESS);
		}

		case CommandApdu::CHIP_AUTHENTICATION:
		{
			if (protocol.getProtocol() != ProtocolType::CA && protocol.getProtocol() != ProtocolType::RI)
			{
				return ResponseApdu(StatusCode::REFERENCED_DATA_NOT_FOUND);
			}

			mCardKey = mFileSystem.getKey(cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::PRIVATE_KEY_REFERENCE).back());

			return ResponseApdu(StatusCode::SUCCESS);
		}

		case CommandApdu::TERMINAL_AUTHENTICATION:
		{
			if (protocol.getProtocol() != ProtocolType::TA)
			{
				return ResponseApdu(StatusCode::REFERENCED_DATA_NOT_FOUND);
			}

			const auto& chr = cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::PUBLIC_KEY_REFERENCE);
			if (!mTaCertificate || mTaCertificate->getBody().getCertificateHolderReference() != chr)
			{
				return ResponseApdu(StatusCode::REFERENCED_DATA_NOT_FOUND);
			}

			if (mTaCertificate->getBody().getHashAlgorithm() != protocol.getHashAlgorithm())
			{
				return ResponseApdu(StatusCode::NO_PARENT_FILE);
			}

			mTaAuxData = cmdData.getObject(V_ASN1_APPLICATION, ASN1Struct::AUXILIARY_AUTHENTICATED_DATA);
			mTaSigningData.append(cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::TA_EPHEMERAL_PUBLIC_KEY));

			return ResponseApdu(StatusCode::SUCCESS);
		}

		default:
			return ResponseApdu(StatusCode::INVALID_PARAMETER);
	}
}


ResponseApdu SimulatorCard::executeMseSetDst(const QByteArray& pData) const
{
	const auto& pubKey = ASN1Struct(pData).getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::PUBLIC_KEY_REFERENCE);
	if (mTaCertificate && mTaCertificate->getBody().getCertificateHolderReference() == pubKey)
	{
		return ResponseApdu(StatusCode::SUCCESS);
	}

	return ResponseApdu(StatusCode::REFERENCED_DATA_NOT_FOUND);
}


ResponseApdu SimulatorCard::executeGeneralAuthenticate(const CommandApdu& pCmd)
{
	if (pCmd.getData().isEmpty())
	{
		return ResponseApdu(StatusCode::WRONG_LENGTH);
	}

	const ASN1Struct cmdData(pCmd.getData());

	if (mSelectedProtocol == KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128)
	{
		const SecurityProtocol protocol(mSelectedProtocol);
		QByteArray responseData;

		if (mChainingStep <= 2 && !pCmd.isCommandChaining())
		{
			return ResponseApdu(StatusCode::UNSUPPORTED_CLA);
		}

		switch (mChainingStep)
		{
			case 0:
			{
				mPaceNonce = Randomizer::getInstance().createBytes(16);
				const auto& symmetricKey = KeyDerivationFunction(protocol).pi(mFileSystem.getPassword(mPacePassword));
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
				const auto& encodedPublicKey = EcUtil::getEncodedPublicKey(mCardKey);
				Asn1OctetStringUtil::setValue(encodedPublicKey, asn1KeyAgreement->mEphemeralPublicKey);
				responseData = encodeObject(asn1KeyAgreement.data());
				break;
			}

			case 3:
			{
				if (pCmd.isCommandChaining())
				{
					mChainingStep = -1;
					return ResponseApdu(StatusCode::LAST_CHAIN_CMD_EXPECTED);
				}

				const auto guard = qScopeGuard([this] {mCardKey.reset();});
				const auto& mutualAuthenticationDataCard = cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::AUTHENTICATION_TOKEN);
				const auto& mutualAuthenticationDataTerminal = generateAuthenticationToken(mPaceTerminalKey, QByteArray(), mutualAuthenticationDataCard);
				mPaceTerminalKey.clear();

				if (mutualAuthenticationDataTerminal.isNull())
				{
					return ResponseApdu(StatusCode::PIN_RETRY_COUNT_2);
				}

				auto ga = newObject<GA_MUTUALAUTHENTICATIONDATA>();
				Asn1OctetStringUtil::setValue(mutualAuthenticationDataTerminal, ga->mAuthenticationToken);
				if (mTaCertificate)
				{
					mTaSigningData = EcUtil::getEncodedPublicKey(mCardKey, true);

					ga->mCarCurr = ASN1_OCTET_STRING_new();
					Asn1OctetStringUtil::setValue(mTaCertificate->getBody().getCertificateHolderReference(), ga->mCarCurr);
				}
				responseData = encodeObject(ga.data());
				break;
			}

			default:
				Q_UNREACHABLE();
		}

		return ResponseApdu(responseData + QByteArray::fromHex("9000"));
	}

	if (mSelectedProtocol == KnownOid::ID_CA_ECDH_AES_CBC_CMAC_128)
	{
		const auto& caKey = cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::CA_EPHEMERAL_PUBLIC_KEY);
		if (caKey.isEmpty())
		{
			return ResponseApdu(StatusCode::INVALID_DATAFIELD);
		}

		const QByteArray nonce = QByteArray::fromHex("0001020304050607");
		const QByteArray& authenticationToken = generateAuthenticationToken(caKey, nonce);

		auto ga = newObject<GA_CHIPAUTHENTICATIONDATA>();
		Asn1OctetStringUtil::setValue(nonce, ga->mNonce);
		Asn1OctetStringUtil::setValue(authenticationToken, ga->mAuthenticationToken);
		return ResponseApdu(encodeObject(ga.data()) + QByteArray::fromHex("9000"));
	}

	if (mSelectedProtocol == KnownOid::ID_RI_ECDH_SHA_256)
	{
		const auto& rawOid = cmdData.getData(V_ASN1_UNIVERSAL, ASN1Struct::UNI_OBJECT_IDENTIFIER);
		const auto& riKey = cmdData.getData(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::EC_PUBLIC_POINT);
		if (rawOid.isEmpty() || riKey.isEmpty())
		{
			return ResponseApdu(StatusCode::INVALID_DATAFIELD);
		}

		const Oid oid(rawOid);
		if (oid != KnownOid::ID_RI_ECDH_SHA_256)
		{
			return ResponseApdu(StatusCode::NO_BINARY_FILE);
		}

		const QByteArray& restrictedId = generateRestrictedId(riKey);

		const auto responseData = Asn1Util::encode(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::RI_FIRST_IDENTIFIER, restrictedId);
		const auto response = Asn1Util::encode(V_ASN1_APPLICATION, ASN1Struct::DYNAMIC_AUTHENTICATION_DATA, responseData, true);
		return ResponseApdu(response + QByteArray::fromHex("9000"));
	}

	return ResponseApdu(StatusCode::VERIFICATION_FAILED);
}


ResponseApdu SimulatorCard::executePsoVerify(const QByteArray& pData)
{
	const auto rawCertificate = Asn1Util::encode(V_ASN1_APPLICATION, ASN1Struct::CV_CERTIFICATE, pData, true);
	const auto& certificate = CVCertificate::fromRaw(rawCertificate);
	const auto& trustPoint = mFileSystem.getTrustPoint();
	if (!SignatureChecker::checkSignature(certificate, mTaCertificate, &trustPoint->getBody().getPublicKey()))
	{
		return ResponseApdu(StatusCode::VERIFICATION_FAILED);
	}

	switch (certificate->getBody().getCHAT().getAccessRole())
	{
		case EnumAccessRole::AccessRole::CVCA:
			mFileSystem.setTrustPoint(certificate);
			break;

		case EnumAccessRole::AccessRole::AT:
			mAccessRights.intersect(trustPoint->getBody().getCHAT().getAccessRights());
			mAccessRights.intersect(mTaCertificate->getBody().getCHAT().getAccessRights());
			mAccessRights.intersect(certificate->getBody().getCHAT().getAccessRights());
			break;

		default:
			break;
	}
	mTaCertificate = certificate;

	return ResponseApdu(StatusCode::SUCCESS);
}


ResponseApdu SimulatorCard::executeExternalAuthenticate(const QByteArray& pSignature)
{
	if (pSignature.isEmpty())
	{
		return ResponseApdu(StatusCode::WRONG_LENGTH);
	}

	SecurityProtocol protocol(mSelectedProtocol);
	if (protocol.getProtocol() == ProtocolType::TA && protocol.getSignature() == SignatureType::ECDSA)
	{
		if (protocol.getHashAlgorithm() != mTaCertificate->getBody().getHashAlgorithm())
		{
			return ResponseApdu(StatusCode::NO_PARENT_FILE);
		}

		const auto guard = qScopeGuard([this] {mTaSigningData.clear();});
		const auto& terminalPublicKey = mFileSystem.getTrustPoint()->getBody().getPublicKey().createKey(mTaCertificate->getBody().getPublicKey().getUncompressedPublicPoint());
		mTaSigningData.append(mTaAuxData);
		if (SignatureChecker::checkSignature(terminalPublicKey, pSignature, mTaSigningData, protocol.getHashAlgorithm()))
		{
			return ResponseApdu(StatusCode::SUCCESS);
		}

		return ResponseApdu(StatusCode::VERIFICATION_FAILED);

	}

	return ResponseApdu(StatusCode::NO_PARENT_FILE);
}


ResponseApdu SimulatorCard::executePinManagement(const CommandApdu& pCmd) const
{
	if (pCmd.getINS() != Ins::ACTIVATE && pCmd.getINS() != Ins::DEACTIVATE)
	{
		return ResponseApdu(StatusCode::UNSUPPORTED_INS);
	}

	if (pCmd.getP1() != 0x10 || pCmd.getP2() != CommandApdu::PIN)
	{
		return ResponseApdu(StatusCode::INVALID_PARAMETER);
	}

	qCDebug(card_simulator) << pCmd.getINS() << "PIN";
	return ResponseApdu(StatusCode::SUCCESS);
}


ResponseApdu SimulatorCard::executeResetRetryCounter(const CommandApdu& pCmd) const
{
	if (pCmd.getINS() != Ins::RESET_RETRY_COUNTER)
	{
		return ResponseApdu(StatusCode::UNSUPPORTED_INS);
	}

	if (pCmd.getP2() == CommandApdu::PIN)
	{
		if (pCmd.getP1() == CommandApdu::CHANGE)
		{
			qCDebug(card_simulator) << "CHANGE PIN:" << pCmd.getData();
			return ResponseApdu(StatusCode::SUCCESS);
		}

		if (pCmd.getP1() == CommandApdu::UNBLOCK)
		{
			qCDebug(card_simulator) << "UNBLOCK PIN";
			return ResponseApdu(StatusCode::SUCCESS);
		}
	}

	return ResponseApdu(StatusCode::INVALID_PARAMETER);
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
	const auto& privateKey = EcUtil::getPrivateKey(mCardKey);
	if (!EC_POINT_mul(curve.data(), result.data(), nullptr, point.data(), privateKey.data(), nullptr))
	{
		qCCritical(card_simulator) << "Calculation of the result failed";
		return QByteArray();
	}

	return EcUtil::point2oct(curve, result.data(), true);

#endif
}


QByteArray SimulatorCard::generateAuthenticationToken(const QByteArray& pPublicKey, const QByteArray& pNonce, const QByteArray& pVerify)
{
	QByteArray sharedSecret = ecMultiplication(pPublicKey);

	const auto protocol = SecurityProtocol(mSelectedProtocol);
	KeyDerivationFunction kdf(protocol);

	QByteArray macKey = kdf.mac(sharedSecret, pNonce);
	CipherMac cmac(protocol, macKey);
	if (!pVerify.isNull())
	{
		const auto& uncompressedCardPublicKey = EcdhKeyAgreement::encodeUncompressedPublicKey(mSelectedProtocol, EcUtil::getEncodedPublicKey(mCardKey));
		const auto& mutualAuthenticationCardData = cmac.generate(uncompressedCardPublicKey);
		if (pVerify != mutualAuthenticationCardData)
		{
			return QByteArray();
		}
	}

	QByteArray encKey = kdf.enc(sharedSecret, pNonce);
	mNewSecureMessaging.reset(new SecureMessaging(protocol, encKey, macKey));
	return cmac.generate(EcdhKeyAgreement::encodeUncompressedPublicKey(mSelectedProtocol, pPublicKey));
}


QByteArray SimulatorCard::generateRestrictedId(const QByteArray& pPublicKey) const
{
	QByteArray sharedSecret = ecMultiplication(pPublicKey);

	return QCryptographicHash::hash(sharedSecret, QCryptographicHash::Sha256);
}


StatusCode SimulatorCard::verifyAuxiliaryData(const QByteArray& pASN1Struct) const
{
	const auto* unsignedCharPointer = reinterpret_cast<const uchar*>(pASN1Struct.constData());
	ASN1_OBJECT* obj = d2i_ASN1_OBJECT(nullptr, &unsignedCharPointer, static_cast<long>(pASN1Struct.size()));
	const auto guard = qScopeGuard([obj] {ASN1_OBJECT_free(obj);});
	return mFileSystem.verify(Oid(obj), AuthenticatedAuxiliaryData::decode(mTaAuxData));
}
