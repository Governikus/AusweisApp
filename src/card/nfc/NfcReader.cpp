/*!
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

#include "NfcReader.h"

#include "CardConnectionWorker.h"
#include "VolatileSettings.h"

#include <QLoggingCategory>

#if defined(Q_OS_ANDROID)
#include <QtAndroid>
#endif

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


Reader::CardEvent NfcReader::updateCard()
{
	return CardEvent::NONE;
}


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
	mReaderInfo.setMaxApduLength(length);
	if (!mReaderInfo.sufficientApduLength())
	{
		Q_EMIT fireReaderPropertiesUpdated(mReaderInfo);
		qCDebug(card_nfc) << "ExtendedLengthApduSupport missing. MaxTransceiveLength:" << length;
	}

	mCard.reset(new NfcCard(pTarget));
	connect(mCard.data(), &NfcCard::fireSetProgressMessage, this, &NfcReader::setProgressMessage);
	QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
	CardInfoFactory::create(cardConnection, mReaderInfo);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	//: INFO IOS Feedback when a new ID card has been detected
	const auto& info = Env::getSingleton<VolatileSettings>()->isUsedAsSDK()
			 ? Env::getSingleton<VolatileSettings>()->getMessages().getSessionInProgress()
			 : tr("ID card detected. Please do not move the device!");

	mNfManager.setUserInformation(info);
#endif
	Q_EMIT fireCardInserted(mReaderInfo);
}


void NfcReader::targetLost(QNearFieldTarget* pTarget)
{
	qCDebug(card_nfc) << "targetLost";
	if (pTarget != nullptr && mCard && mCard->invalidateTarget(pTarget))
	{
		mCard.reset();
		mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
		Q_EMIT fireCardRemoved(mReaderInfo);
	}
}


void NfcReader::setProgressMessage(const QString& pMessage)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	const auto& info = Env::getSingleton<VolatileSettings>()->isUsedAsSDK()
			 ? Env::getSingleton<VolatileSettings>()->getMessages().getSessionInProgress()
			 : pMessage;

	mNfManager.setUserInformation(info);
#else
	Q_UNUSED(pMessage)
#endif
}


NfcReader::NfcReader()
	: ConnectableReader(ReaderManagerPlugInType::NFC, QStringLiteral("NFC"))
	, mNfManager()
{
	mReaderInfo.setBasicReader(true);
	mReaderInfo.setConnected(true);

	connect(&mNfManager, &QNearFieldManager::adapterStateChanged, this, &NfcReader::adapterStateChanged);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	connect(&mNfManager, &QNearFieldManager::targetDetectionStopped, this, &NfcReader::fireReaderDisconnected);
#endif
	connect(&mNfManager, &QNearFieldManager::targetDetected, this, &NfcReader::targetDetected);
	connect(&mNfManager, &QNearFieldManager::targetLost, this, &NfcReader::targetLost);

#if defined(Q_OS_ANDROID)
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	mNfManager.startTargetDetection();
#else
	mNfManager.startTargetDetection(QNearFieldTarget::TagTypeSpecificAccess);
#endif
	auto activity = QtAndroid::androidActivity();
	// Check if not used as SDK
	if (activity.isValid())
	{
		activity.callMethod<void>("enableNfcReaderMode");
	}
#endif
}


NfcReader::~NfcReader()
{
#if defined(Q_OS_ANDROID)
	auto activity = QtAndroid::androidActivity();
	// Check if not used as SDK
	if (activity.isValid())
	{
		activity.callMethod<void>("disableNfcReaderMode");
	}
	mNfManager.stopTargetDetection();
#endif
}


bool NfcReader::isEnabled() const
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	return mNfManager.isAvailable();

#else
	return mNfManager.isEnabled();

#endif
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
#if defined(Q_OS_IOS) && QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
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
#if defined(Q_OS_IOS) && QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
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
