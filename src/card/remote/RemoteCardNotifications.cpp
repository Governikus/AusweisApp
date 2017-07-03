/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */


#include "RemoteCardNotifications.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QVector>


Q_DECLARE_LOGGING_CATEGORY(card_remote)

using namespace governikus;


QLatin1String RemoteCardNotification::getTypeName() const
{
	return getEnumName(getType());
}


RemoteCardNotification::RemoteCardNotification()
{
}


RemoteCardNotification::~RemoteCardNotification()
{
}


RemoteReaderDiscoverCmd::RemoteReaderDiscoverCmd()
{
}


CardNotificationType RemoteReaderDiscoverCmd::getType() const
{
	return CardNotificationType::REMOTE_READER_DISCOVER;
}


QJsonDocument RemoteReaderDiscoverCmd::toJson() const
{
	static const QLatin1String CMD("cmd");

	QJsonObject result;

	result[CMD] = getTypeName();

	return QJsonDocument(result);
}


void RemoteReaderDiscoverCmd::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<RemoteReaderDiscoverCmd> message(new RemoteReaderDiscoverCmd(*this));

	pVisitor->process(message);
}


RemoteReaderOfferMsg::RemoteReaderOfferMsg(const QString& pDeviceName, bool pEncrypted, quint16 pPort, const QVector<int>& pAvailableApiLevels)
	: mDeviceName(pDeviceName)
	, mEncrypted(pEncrypted)
	, mPort(pPort)
	, mAvailableApiLevels(pAvailableApiLevels)
{
}


const QString& RemoteReaderOfferMsg::getDeviceName() const
{
	return mDeviceName;
}


bool RemoteReaderOfferMsg::isEncrypted() const
{
	return mEncrypted;
}


quint16 RemoteReaderOfferMsg::getPort() const
{
	return mPort;
}


const QVector<int>& RemoteReaderOfferMsg::getAvailableApiLevels() const
{
	return mAvailableApiLevels;
}


CardNotificationType RemoteReaderOfferMsg::getType() const
{
	return CardNotificationType::REMOTE_READER_OFFER;
}


QJsonDocument RemoteReaderOfferMsg::toJson() const
{
	static const QLatin1String MSG("msg");
	static const QLatin1String DEVICE_NAME("deviceName");
	static const QLatin1String ENCRYPTED("encrypted");
	static const QLatin1String PORT("port");
	static const QLatin1String AVAILABLE_API_LEVELS("availableApiLevels");

	QJsonObject result;

	result[MSG] = getTypeName();
	result[DEVICE_NAME] = mDeviceName;
	result[ENCRYPTED] = mEncrypted;
	result[PORT] = mPort;

	QJsonArray levels;
	for (auto level : mAvailableApiLevels)
	{
		levels.append(level);
	}
	result[AVAILABLE_API_LEVELS] = levels;

	return QJsonDocument(result);
}


void RemoteReaderOfferMsg::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<RemoteReaderOfferMsg> message(new RemoteReaderOfferMsg(*this));

	pVisitor->process(message);
}


bool RemoteReaderOfferMsg::operator==(const RemoteReaderOfferMsg& pOther) const
{
	return mDeviceName == pOther.mDeviceName
		   && mEncrypted == pOther.mEncrypted
		   && mPort == pOther.mPort
		   && mAvailableApiLevels == pOther.mAvailableApiLevels;
}


bool RemoteReaderOfferMsg::isEqual(const RemoteReaderOfferMsg* const pOther) const
{
	return *this == *pOther;
}


GetApiLevelCmd::GetApiLevelCmd()
{
}


CardNotificationType GetApiLevelCmd::getType() const
{
	return CardNotificationType::GET_API_LEVEL;
}


QJsonDocument GetApiLevelCmd::toJson() const
{
	static const QLatin1String CMD("cmd");

	QJsonObject result;

	result[CMD] = getTypeName();

	return QJsonDocument(result);
}


void GetApiLevelCmd::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<GetApiLevelCmd> message(new GetApiLevelCmd(*this));

	pVisitor->process(message);
}


SetApiLevelCmd::SetApiLevelCmd(int pLevel)
	: mLevel(pLevel)
{
}


int SetApiLevelCmd::getLevel() const
{
	return mLevel;
}


CardNotificationType SetApiLevelCmd::getType() const
{
	return CardNotificationType::SET_API_LEVEL;
}


QJsonDocument SetApiLevelCmd::toJson() const
{
	static const QLatin1String CMD("cmd");
	static const QLatin1String LEVEL("level");

	QJsonObject result;

	result[CMD] = getTypeName();
	result[LEVEL] = mLevel;

	return QJsonDocument(result);
}


void SetApiLevelCmd::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<SetApiLevelCmd> message(new SetApiLevelCmd(*this));

	pVisitor->process(message);
}


ApiLevelMsg::ApiLevelMsg(const QString& pError, const QVector<int>& pAvailable, int pCurrent)
	: mError(pError)
	, mAvailable(pAvailable)
	, mCurrent(pCurrent)
{
}


const QString& ApiLevelMsg::getError() const
{
	return mError;
}


const QVector<int>& ApiLevelMsg::getAvailable() const
{
	return mAvailable;
}


int ApiLevelMsg::getCurrent() const
{
	return mCurrent;
}


CardNotificationType ApiLevelMsg::getType() const
{
	return CardNotificationType::API_LEVEL;
}


QJsonDocument ApiLevelMsg::toJson() const
{
	static const QLatin1String MSG("msg");
	static const QLatin1String ERROR("error");
	static const QLatin1String AVAILABLE("available");
	static const QLatin1String CURRENT("current");

	QJsonObject result;

	result[MSG] = getTypeName();
	result[ERROR] = mError;

	QJsonArray levels;
	for (auto level : mAvailable)
	{
		levels.append(level);
	}
	result[AVAILABLE] = levels;
	result[CURRENT] = mCurrent;

	return QJsonDocument(result);
}


void ApiLevelMsg::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<ApiLevelMsg> message(new ApiLevelMsg(*this));

	pVisitor->process(message);
}


GetReaderListCmd::GetReaderListCmd()
{
}


CardNotificationType GetReaderListCmd::getType() const
{
	return CardNotificationType::GET_READER_LIST;
}


QJsonDocument GetReaderListCmd::toJson() const
{
	static const QLatin1String CMD("cmd");

	QJsonObject result;

	result[CMD] = getTypeName();

	return QJsonDocument(result);
}


void GetReaderListCmd::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<GetReaderListCmd> message(new GetReaderListCmd(*this));

	pVisitor->process(message);
}


CardDescription::CardDescription(const QByteArray& pEFCardAccess, int pRetryCounter, bool pPinDeactivated, bool pConnected)
	: mEFCardAccess(pEFCardAccess)
	, mRetryCounter(pRetryCounter)
	, mPinDeactivated(pPinDeactivated)
	, mConnected(pConnected)
{
}


const QByteArray& CardDescription::getEFCardAccess() const
{
	return mEFCardAccess;
}


int CardDescription::getRetryCounter() const
{
	return mRetryCounter;
}


bool CardDescription::isPinDeactivated() const
{
	return mPinDeactivated;
}


bool CardDescription::isConnected() const
{
	return mConnected;
}


QJsonValue CardDescription::toJson() const
{
	static const QLatin1String EF_CARD_ACCESS("EFCardAccess");
	static const QLatin1String RETRY_COUNTER("retryCounter");
	static const QLatin1String PIN_DEACTIVATED("pinDeactivated");
	static const QLatin1String CONNECTED("connected");

	if (mEFCardAccess.isEmpty())
	{
		// null
		return QJsonValue();
	}
	else
	{
		QJsonObject result;

		result[EF_CARD_ACCESS] = QString::fromLatin1(mEFCardAccess.toHex());
		result[RETRY_COUNTER] = mRetryCounter;
		result[PIN_DEACTIVATED] = mPinDeactivated;
		result[CONNECTED] = mConnected;

		return result;
	}
}


bool CardDescription::operator==(const CardDescription& pOther) const
{
	return mEFCardAccess == pOther.mEFCardAccess &&
		   mRetryCounter == pOther.mRetryCounter &&
		   mPinDeactivated == pOther.mPinDeactivated &&
		   mConnected == pOther.mConnected;
}


ReaderDescription::ReaderDescriptionData::ReaderDescriptionData(const QString& pName,
		bool pAttached,
		ExtendedLengthApduSupportCode pExtendedLength,
		const CardDescription& pCard)
	: mName(pName)
	, mAttached(pAttached)
	, mExtendedLength(pExtendedLength)
	, mCard(pCard)
{
}


bool ReaderDescription::ReaderDescriptionData::operator==(const ReaderDescriptionData& pOther) const
{
	return mName == pOther.mName &&
		   mAttached == pOther.mAttached &&
		   mExtendedLength == pOther.mExtendedLength &&
		   mCard == pOther.mCard;
}


ReaderDescription::ReaderDescription(const QString& pName,
		bool pAttached,
		ExtendedLengthApduSupportCode pExtendedLength,
		const CardDescription& pCard)
	: d(new ReaderDescriptionData(pName, pAttached, pExtendedLength, pCard))
{
}


bool ReaderDescription::hasCard() const
{
	return !d->mCard.getEFCardAccess().isEmpty();
}


const QString& ReaderDescription::getName() const
{
	return d->mName;
}


bool ReaderDescription::isAttached() const
{
	return d->mAttached;
}


ExtendedLengthApduSupportCode ReaderDescription::getExtendedLength() const
{
	return d->mExtendedLength;
}


const CardDescription& ReaderDescription::getCard() const
{
	return d->mCard;
}


QJsonObject ReaderDescription::toJson() const
{
	static const QLatin1String NAME("name");
	static const QLatin1String ATTACHED("attached");
	static const QLatin1String EXTENDED_LENGTH("extendedLength");
	static const QLatin1String CARD("card");

	QJsonObject result;

	result[NAME] = d->mName;
	result[ATTACHED] = d->mAttached;
	result[EXTENDED_LENGTH] = getEnumName(d->mExtendedLength);
	result[CARD] = d->mCard.toJson();

	return result;
}


bool ReaderDescription::operator==(const ReaderDescription& pOther) const
{
	return *d == *pOther.d;
}


ReaderListMsg::ReaderListMsg(const QVector<ReaderDescription>& pReaders)
	: mReaders(pReaders)
{
}


const QVector<ReaderDescription>& ReaderListMsg::getReaders() const
{
	return mReaders;
}


CardNotificationType ReaderListMsg::getType() const
{
	return CardNotificationType::READER_LIST;
}


QJsonDocument ReaderListMsg::toJson() const
{
	static const QLatin1String MSG("msg");
	static const QLatin1String READERS("readers");

	QJsonObject result;

	result[MSG] = getTypeName();

	QJsonArray readers;
	for (auto reader : mReaders)
	{
		readers.append(reader.toJson());
	}
	result[READERS] = readers;

	return QJsonDocument(result);
}


void ReaderListMsg::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<ReaderListMsg> message(new ReaderListMsg(*this));

	pVisitor->process(message);
}


ReaderMsg::ReaderMsg(const ReaderDescription& pReader, const QString& pError)
	: mReader(pReader)
	, mError(pError)
{
}


const ReaderDescription& ReaderMsg::getReader() const
{
	return mReader;
}


const QString& ReaderMsg::getError() const
{
	return mError;
}


CardNotificationType ReaderMsg::getType() const
{
	return CardNotificationType::READER;
}


QJsonDocument ReaderMsg::toJson() const
{
	static const QLatin1String MSG("msg");
	static const QLatin1String READER("reader");
	static const QLatin1String ERROR("error");

	QJsonObject result;

	result[MSG] = getTypeName();
	result[READER] = mReader.toJson();
	result[ERROR] = mError;

	return QJsonDocument(result);
}


void ReaderMsg::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<ReaderMsg> message(new ReaderMsg(*this));

	pVisitor->process(message);
}


ConnectCmd::ConnectCmd(const QString& pReaderName)
	: mReaderName(pReaderName)
{
}


const QString& ConnectCmd::getReaderName() const
{
	return mReaderName;
}


CardNotificationType ConnectCmd::getType() const
{
	return CardNotificationType::CONNECT;
}


QJsonDocument ConnectCmd::toJson() const
{
	static const QLatin1String CMD("cmd");
	static const QLatin1String READER_NAME("readerName");

	QJsonObject result;

	result[CMD] = getTypeName();
	result[READER_NAME] = mReaderName;

	return QJsonDocument(result);
}


void ConnectCmd::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<ConnectCmd> message(new ConnectCmd(*this));

	pVisitor->process(message);
}


DisconnectCmd::DisconnectCmd(const QString& pReaderName)
	: mReaderName(pReaderName)
{
}


const QString& DisconnectCmd::getReaderName() const
{
	return mReaderName;
}


CardNotificationType DisconnectCmd::getType() const
{
	return CardNotificationType::DISCONNECT;
}


QJsonDocument DisconnectCmd::toJson() const
{
	static const QLatin1String CMD("cmd");
	static const QLatin1String READER_NAME("readerName");

	QJsonObject result;

	result[CMD] = getTypeName();
	result[READER_NAME] = mReaderName;

	return QJsonDocument(result);
}


void DisconnectCmd::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<DisconnectCmd> message(new DisconnectCmd(*this));

	pVisitor->process(message);
}


TransmitCmd::TransmitCmd(const QString& pReaderName, const QByteArray& pCommandApdu)
	: mReaderName(pReaderName)
	, mCommandApdu(pCommandApdu)
{
}


const QString& TransmitCmd::getReaderName() const
{
	return mReaderName;
}


const QByteArray& TransmitCmd::getCommandApdu() const
{
	return mCommandApdu;
}


CardNotificationType TransmitCmd::getType() const
{
	return CardNotificationType::TRANSMIT;
}


QJsonDocument TransmitCmd::toJson() const
{
	static const QLatin1String CMD("cmd");
	static const QLatin1String READER_NAME("readerName");
	static const QLatin1String COMMAND_APDU("commandApdu");

	QJsonObject result;

	result[CMD] = getTypeName();
	result[READER_NAME] = mReaderName;
	result[COMMAND_APDU] = QString::fromLatin1(mCommandApdu.toHex());

	return QJsonDocument(result);
}


void TransmitCmd::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<TransmitCmd> message(new TransmitCmd(*this));

	pVisitor->process(message);
}


TransmitResponseMsg::TransmitResponseMsg(const QString& pReaderName, const QString& pError, const QByteArray& pResponseApdu)
	: mReaderName(pReaderName)
	, mError(pError)
	, mResponseApdu(pResponseApdu)
{
}


const QString& TransmitResponseMsg::getReaderName() const
{
	return mReaderName;
}


const QString& TransmitResponseMsg::getError() const
{
	return mError;
}


const QByteArray& TransmitResponseMsg::getResponseApdu() const
{
	return mResponseApdu;
}


CardNotificationType TransmitResponseMsg::getType() const
{
	return CardNotificationType::TRANSMIT_RESPONSE;
}


QJsonDocument TransmitResponseMsg::toJson() const
{
	static const QLatin1String MSG("msg");
	static const QLatin1String READER_NAME("readerName");
	static const QLatin1String ERROR("error");
	static const QLatin1String RESPONSE_APDU("responseApdu");

	QJsonObject result;

	result[MSG] = getTypeName();
	result[READER_NAME] = mReaderName;
	result[ERROR] = mError;
	result[RESPONSE_APDU] = QString::fromLatin1(mResponseApdu.toHex());

	return QJsonDocument(result);
}


void TransmitResponseMsg::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<TransmitResponseMsg> message(new TransmitResponseMsg(*this));

	pVisitor->process(message);
}


UnsupportedMsg::UnsupportedMsg(const QString& pError)
	: mError(pError)
{
}


const QString& UnsupportedMsg::getError() const
{
	return mError;
}


CardNotificationType UnsupportedMsg::getType() const
{
	return CardNotificationType::UNSUPPORTED;
}


QJsonDocument UnsupportedMsg::toJson() const
{
	static const QLatin1String MSG("msg");
	static const QLatin1String ERROR("error");

	QJsonObject result;

	result[MSG] = getEnumName(getType());
	result[ERROR] = mError;

	return QJsonDocument(result);
}


void UnsupportedMsg::accept(const QSharedPointer<NotificationVisitor>& pVisitor) const
{
	const QSharedPointer<UnsupportedMsg> message(new UnsupportedMsg(*this));

	pVisitor->process(message);
}


void NotificationVisitor::process(const QSharedPointer<RemoteReaderDiscoverCmd>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<RemoteReaderOfferMsg>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<GetApiLevelCmd>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<SetApiLevelCmd>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<ApiLevelMsg>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<GetReaderListCmd>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<ReaderListMsg>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<ReaderMsg>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<ConnectCmd>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<DisconnectCmd>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<TransmitCmd>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<TransmitResponseMsg>& /* pNotification */)
{
}


void NotificationVisitor::process(const QSharedPointer<UnsupportedMsg>& /* pNotification */)
{
}


#include "moc_RemoteCardNotifications.cpp"
