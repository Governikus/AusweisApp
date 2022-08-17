/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "SimulatorCard.h"

#include "FileRef.h"
#include "VolatileSettings.h"
#include "apdu/FileCommand.h"
#include "apdu/GeneralAuthenticateResponse.h"
#include "apdu/SecureMessagingResponse.h"
#include "asn1/ASN1TemplateUtil.h"
#include "asn1/ASN1Util.h"
#include "pace/CipherMac.h"
#include "pace/KeyDerivationFunction.h"
#include "pace/ec/EcUtil.h"

#include <QCryptographicHash>
#include <QScopeGuard>
#include <QThread>
#include <QtEndian>
#include <openssl/bn.h>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(secure)


SimulatorCard::SimulatorCard(const SimulatorFileSystem& pFileSystem)
	: Card()
	, mConnected(false)
	, mFileSystem(pFileSystem)
	, mAuxiliaryData()
	, mSecureMessaging()
	, mNewSecureMessaging()
	, mRiKeyId(0)
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
	QByteArray commandBytes = pCmd;
	ResponseApduResult result = {CardReturnCode::OK, ResponseApdu(StatusCode::SUCCESS)};

	qCDebug(card) << "Transmit command APDU:" << commandApdu;

	if (mSecureMessaging)
	{
		if (commandApdu.isSecureMessaging())
		{
			commandApdu = mSecureMessaging->decrypt(commandApdu);
			commandBytes = commandApdu;
			qCDebug(secure) << "Unencrypted transmit command APDU:" << commandApdu;
		}
		else
		{
			qCDebug(card) << "Received unencrypted command APDU. Disable secure messaging";
			mSecureMessaging.reset();
		}
	}

	switch (commandApdu.getINS())
	{
		case Ins::SELECT:
		{
			const auto fileRef = FileCommand(commandApdu).getFileRef();
			if (fileRef.getType() == FileRef::ELEMENTARY_FILE)
			{
				const auto& fileId = fileRef.getIdentifier();
				result = {CardReturnCode::OK, ResponseApdu(mFileSystem.select(fileId))};
			}

			break;
		}

		case Ins::READ_BINARY:
		{
			const FileCommand fileCommand(commandApdu);
			const int offset = fileCommand.getOffset();
			const auto fileRef = fileCommand.getFileRef();

			if (offset == 0 && fileRef.getType() == FileRef::TYPE::ELEMENTARY_FILE)
			{
				const auto& selectResult = mFileSystem.select(fileRef.getShortIdentifier());
				result = {CardReturnCode::OK, ResponseApdu(selectResult)};
			}

			if (result.mResponseApdu.getStatusCode() == StatusCode::SUCCESS)
			{
				const auto& file = mFileSystem.read(offset, fileCommand.getLe(), commandApdu.isExtendedLength());
				result = {CardReturnCode::OK, ResponseApdu(file)};
			}

			break;
		}

		case Ins::UPDATE_BINARY:
		{
			const FileCommand fileCommand(commandApdu);
			const int offset = fileCommand.getOffset();
			const auto fileRef = fileCommand.getFileRef();

			if (offset == 0 && fileRef.getType() == FileRef::TYPE::ELEMENTARY_FILE)
			{
				const auto& selectResult = mFileSystem.select(fileRef.getShortIdentifier());
				result = {CardReturnCode::OK, ResponseApdu(selectResult)};
			}

			if (result.mResponseApdu.getStatusCode() == StatusCode::SUCCESS)
			{
				const auto& statusCode = mFileSystem.write(offset, commandApdu.getData());
				result = {CardReturnCode::OK, ResponseApdu(statusCode)};
			}

			break;
		}

		default:
			break;
	}

	// Challenge
	if (commandBytes.startsWith(QByteArray::fromHex("00840000")))
	{
		result = {CardReturnCode::OK, ResponseApdu(QByteArray::fromHex("01020304050607089000"))};
	}

	// MSE:Set AT for Restricted Identification
	if (commandBytes.startsWith(QByteArray::fromHex("002241A4 0F 800A 04007F00070202050203 8401")))
	{
		mRiKeyId = commandBytes.right(1).back();
	}

	// CA General Authenticate - Chip Authentication
	// RE General Authenticate - Restricted Identification
	if (commandBytes.startsWith(QByteArray::fromHex("00860000")))
	{
		const QByteArray& asn1Data = commandApdu.getData();

		auto index = asn1Data.indexOf(QByteArray::fromHex("8041"));
		if (index >= 0)
		{
			const QByteArray ephemeralPublicKey = asn1Data.mid(index + 2, 65);
			const QByteArray nonce = QByteArray::fromHex("0001020304050607");
			const QByteArray& authenticationToken = generateAuthenticationToken(ephemeralPublicKey, nonce);

			auto ga = newObject<GA_CHIPAUTHENTICATIONDATA>();
			Asn1OctetStringUtil::setValue(nonce, ga->mNonce);
			Asn1OctetStringUtil::setValue(authenticationToken, ga->mAuthenticationToken);
			result = {CardReturnCode::OK, ResponseApdu(encodeObject(ga.data()) + QByteArray::fromHex("9000"))};
		}
		else
		{
			index = asn1Data.indexOf(QByteArray::fromHex("8641"));
			if (index >= 0)
			{
				const QByteArray ephemeralPublicKey = asn1Data.mid(index + 2, 65);
				const QByteArray& restrictedId = generateRestrictedId(ephemeralPublicKey);

				const auto responseData = Asn1Util::encode(V_ASN1_CONTEXT_SPECIFIC, 1, restrictedId);
				const auto response = Asn1Util::encode(V_ASN1_APPLICATION, 28, responseData, true);

				result = {CardReturnCode::OK, ResponseApdu(response + QByteArray::fromHex("9000"))};
			}
		}

	}

	// auxiliaryData (TA) for verification
	if (commandBytes.startsWith(QByteArray::fromHex("002281A4")))
	{
		const auto commandData = commandApdu.getData();
		int index = 0;
		while (index < commandData.size() && commandData.at(index) != 0x67)
		{
			index += commandData.at(index + 1) + 2;
		}
		mAuxiliaryData = AuthenticatedAuxiliaryData::decode(commandData.mid(index, commandData.at(index + 1) + 2));
	}

	// Verify
	if (commandBytes.startsWith(QByteArray::fromHex("80208000")))
	{
		result = {CardReturnCode::OK, ResponseApdu(verifyAuxiliaryData(commandApdu.getData()))};
	}

	if (mSecureMessaging)
	{
		result.mResponseApdu = mSecureMessaging->encrypt(result.mResponseApdu);
	}

	qCDebug(card) << "Transmit response APDU:" << result.mResponseApdu;

	if (mNewSecureMessaging)
	{
		qCDebug(card) << "Start to use a new secure messaging channel";
		mNewSecureMessaging.swap(mSecureMessaging);
		mNewSecureMessaging.reset();
	}

	return result;
}


EstablishPaceChannelOutput SimulatorCard::establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription, quint8 pTimeoutSeconds)
{
	Q_UNUSED(pPasswordId)
	Q_UNUSED(pPreferredPinLength)
	Q_UNUSED(pChat)
	Q_UNUSED(pCertificateDescription)
	Q_UNUSED(pTimeoutSeconds)

	QThread::msleep(Env::getSingleton<VolatileSettings>()->getDelay());
	EstablishPaceChannelOutput output(CardReturnCode::OK);
	output.setPaceReturnCode(CardReturnCode::OK);
	output.setStatusMseSetAt(QByteArray::fromHex("9000"));
	output.setEfCardAccess(mFileSystem.getEfCardAccess());
	output.setCarCurr(QByteArray("DETESTeID00005"));
	output.setIdIcc(QByteArray::fromHex("0102030405060708900A0B0C0D0E0F1011121314"));
	return output;
}


CardReturnCode SimulatorCard::destroyPaceChannel()
{
	return CardReturnCode::OK;
}


ResponseApduResult SimulatorCard::setEidPin(quint8 pTimeoutSeconds)
{
	Q_UNUSED(pTimeoutSeconds)

	QThread::msleep(Env::getSingleton<VolatileSettings>()->getDelay());
	return {CardReturnCode::OK, ResponseApdu(QByteArray::fromHex("9000"))};
}


QByteArray SimulatorCard::brainpoolP256r1Multiplication(const QByteArray& pPoint, const QByteArray& pScalar)
{
	auto curve = EcUtil::createCurve(NID_brainpoolP256r1);

	auto point = EcUtil::oct2point(curve, pPoint);
	if (!point)
	{
		qCCritical(card) << "Interpreting the point failed";
	}

	QSharedPointer<BIGNUM> scalar = EcUtil::create(BN_new());
	if (!BN_bin2bn(reinterpret_cast<const uchar*>(pScalar.data()), pScalar.size(), scalar.data()))
	{
		qCCritical(card) << "Interpreting the scalar failed";
	}

	QSharedPointer<EC_POINT> result = EcUtil::create(EC_POINT_new(curve.data()));
	if (!EC_POINT_mul(curve.data(), result.data(), nullptr, point.data(), scalar.data(), nullptr))
	{
		qCCritical(card) << "Calculation of the result failed";
	}

	return EcUtil::point2oct(curve, result.data(), true);
}


QByteArray SimulatorCard::generateAuthenticationToken(const QByteArray& pPublicKey, const QByteArray& pNonce)
{
	const Oid oid(KnownOid::ID_CA_ECDH_AES_CBC_CMAC_128);

	QByteArray sharedSecret = brainpoolP256r1Multiplication(pPublicKey, mFileSystem.getCardAuthenticationKey());

	const auto protocol = SecurityProtocol(oid);
	KeyDerivationFunction kdf(protocol);

	QByteArray macKey = kdf.mac(sharedSecret, pNonce);
	QByteArray encKey = kdf.enc(sharedSecret, pNonce);

	mNewSecureMessaging.reset(new SecureMessaging(protocol, encKey, macKey));

	const QByteArray oID = Asn1Util::encode(V_ASN1_UNIVERSAL, 6, oid.getData());
	const QByteArray publicPoint = Asn1Util::encode(V_ASN1_CONTEXT_SPECIFIC, 6, pPublicKey);
	const QByteArray& publicKey = Asn1Util::encode(V_ASN1_APPLICATION, 73, oID + publicPoint, true);

	CipherMac cmac(protocol, macKey);
	QByteArray mac = cmac.generate(publicKey);

	return mac;
}


QByteArray SimulatorCard::generateRestrictedId(const QByteArray& pPublicKey)
{
	// const Oid oid(KnownOid::ID_RI_ECDH_SHA_256);

	QByteArray sharedSecret = brainpoolP256r1Multiplication(pPublicKey, mFileSystem.getRestrictedIdentificationKey(mRiKeyId));

	return QCryptographicHash::hash(sharedSecret, QCryptographicHash::Sha256);
}


StatusCode SimulatorCard::verifyAuxiliaryData(const QByteArray& pCommandData)
{
	const auto* unsignedCharPointer = reinterpret_cast<const uchar*>(pCommandData.constData());
	ASN1_OBJECT* obj = d2i_ASN1_OBJECT(nullptr, &unsignedCharPointer, pCommandData.size());
	const auto guard = qScopeGuard([obj] {ASN1_OBJECT_free(obj);});
	return mFileSystem.verify(Oid(obj), mAuxiliaryData);
}
