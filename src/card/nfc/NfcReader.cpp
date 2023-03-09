/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "NfcReader.h"

#include "CardConnectionWorker.h"
#include "VolatileSettings.h"

#include <QLoggingCategory>

#if defined(Q_OS_ANDROID)
	#include <QJniObject>
#endif

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


void NfcReader::adapterStateChanged(QNearFieldManager::AdapterState pState)
{
	Q_EMIT fireNfcAdapterStateChanged(pState == QNearFieldManager::AdapterState::Online);
}


void NfcReader::targetDetected(QNearFieldTarget* pTarget)
{
	if (pTarget == nullptr)
	{
		return;
	}
	qCDebug(card_nfc) << "targetDetected, type:" << pTarget->type();

	if (!(pTarget->accessMethods() & QNearFieldTarget::TagTypeSpecificAccess))
	{
		qCDebug(card_nfc) << "The target does not provide commands";
		return;
	}

	int length = pTarget->maxCommandLength();
	setInfoMaxApduLength(length);
	if (getReaderInfo().insufficientApduLength())
	{
		Q_EMIT fireReaderPropertiesUpdated(getReaderInfo());
		qCDebug(card_nfc) << "ExtendedLengthApduSupport missing. MaxTransceiveLength:" << length;
	}

	mCard.reset(new NfcCard(pTarget));
	connect(mCard.data(), &NfcCard::fireSetProgressMessage, this, &NfcReader::setProgressMessage);
	QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
	fetchCardInfo(cardConnection);

	if (!getCard())
	{
		removeCardInfo();
		return;
	}

	switch (pTarget->type())
	{
		case QNearFieldTarget::Type::NfcTagType4A:
			setCardInfoTagType(CardInfo::TagType::NFC_4A);
			break;

		case QNearFieldTarget::Type::NfcTagType4B:
			setCardInfoTagType(CardInfo::TagType::NFC_4B);
			break;

		default:
			setCardInfoTagType(CardInfo::TagType::UNKNOWN);
	}

	const auto& info = Env::getSingleton<VolatileSettings>()->isUsedAsSDK()
			? Env::getSingleton<VolatileSettings>()->getMessages().getSessionInProgress()
	        //: INFO IOS Feedback when a new ID card has been detected
			: tr("ID card detected. Please do not move the device!");

	mNfManager.setUserInformation(info);

	Q_EMIT fireCardInserted(getReaderInfo());
}


void NfcReader::targetLost(QNearFieldTarget* pTarget)
{
	qCDebug(card_nfc) << "targetLost";
	if (pTarget != nullptr && mCard && mCard->invalidateTarget(pTarget))
	{
		mCard.reset();
		removeCardInfo();
		Q_EMIT fireCardRemoved(getReaderInfo());
	}
}


void NfcReader::setProgressMessage(const QString& pMessage)
{
	mNfManager.setUserInformation(pMessage);
}


NfcReader::NfcReader()
	: ConnectableReader(ReaderManagerPlugInType::NFC, QStringLiteral("NFC"))
	, mNfManager()
	, mCard()
{
	setInfoBasicReader(true);

	connect(&mNfManager, &QNearFieldManager::adapterStateChanged, this, &NfcReader::adapterStateChanged);
	connect(&mNfManager, &QNearFieldManager::targetDetectionStopped, this, &NfcReader::fireReaderDisconnected);
	connect(&mNfManager, &QNearFieldManager::targetDetected, this, &NfcReader::targetDetected);
	connect(&mNfManager, &QNearFieldManager::targetLost, this, &NfcReader::targetLost);

#if defined(Q_OS_ANDROID)
	mNfManager.startTargetDetection(QNearFieldTarget::TagTypeSpecificAccess);

	if (QNativeInterface::QAndroidApplication::isActivityContext())
	{
		if (QJniObject activity = QNativeInterface::QAndroidApplication::context(); activity.isValid())
		{
			activity.callMethod<void>("enableNfcReaderMode");
		}
	}
#endif
}


#if defined(Q_OS_ANDROID)
NfcReader::~NfcReader()
{
	if (QNativeInterface::QAndroidApplication::isActivityContext())
	{
		if (QJniObject activity = QNativeInterface::QAndroidApplication::context(); activity.isValid())
		{
			activity.callMethod<void>("disableNfcReaderMode");
		}
	}
	mNfManager.stopTargetDetection();
}


#else
NfcReader::~NfcReader() = default;


#endif

bool NfcReader::isEnabled() const
{
	return mNfManager.isEnabled();
}


Card* NfcReader::getCard() const
{
	if (mCard && mCard->isValid())
	{
		return mCard.data();
	}

	return nullptr;
}


void NfcReader::connectReader()
{
#if defined(Q_OS_IOS)
	//: INFO IOS The ID card may be inserted, the authentication process may be started.
	const auto& info = Env::getSingleton<VolatileSettings>()->isUsedAsSDK()
			 ? Env::getSingleton<VolatileSettings>()->getMessages().getSessionStarted()
			 : tr("Please place your ID card on the top of the device's back side.");
	mNfManager.setUserInformation(info);
	mNfManager.startTargetDetection(QNearFieldTarget::TagTypeSpecificAccess);
#endif
}


void NfcReader::disconnectReader(const QString& pError)
{
#if defined(Q_OS_IOS)
	if (pError.isNull())
	{
		//: INFO IOS The current session was stopped without errors.
		const auto& info = Env::getSingleton<VolatileSettings>()->isUsedAsSDK()
				 ? Env::getSingleton<VolatileSettings>()->getMessages().getSessionSucceeded()
				 : tr("Scanning process has been finished successfully.");
		mNfManager.setUserInformation(info);
	}
	mNfManager.stopTargetDetection(pError);
#else
	Q_UNUSED(pError)
#endif
}
