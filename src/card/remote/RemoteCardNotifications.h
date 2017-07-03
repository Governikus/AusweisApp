/*!
 * RemoteCardNotifications.h
 *
 * \brief Classes that model remote card reader notifications.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"
#include "ExtendedLengthApduSupportCode.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSharedData>
#include <QSharedDataPointer>
#include <QSharedPointer>
#include <QString>
#include <QVector>

namespace governikus
{
defineEnumType(CardNotificationType,
		REMOTE_READER_DISCOVER,
		REMOTE_READER_OFFER,
		GET_API_LEVEL,
		SET_API_LEVEL,
		API_LEVEL,
		GET_READER_LIST,
		READER_LIST,
		READER,
		CONNECT,
		DISCONNECT,
		TRANSMIT,
		TRANSMIT_RESPONSE,
		UNSUPPORTED)

class NotificationVisitor;


class RemoteCardNotification
{
	protected:
		QLatin1String getTypeName() const;

	public:
		RemoteCardNotification();

		virtual ~RemoteCardNotification();

		virtual CardNotificationType getType() const = 0;

		virtual QJsonDocument toJson() const = 0;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const = 0;
};


class RemoteReaderDiscoverCmd
	: public RemoteCardNotification
{
	public:
		RemoteReaderDiscoverCmd();

		virtual ~RemoteReaderDiscoverCmd() = default;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class RemoteReaderOfferMsg
	: public RemoteCardNotification
{
	private:
		const QString mDeviceName;

		const bool mEncrypted;

		const quint16 mPort;

		const QVector<int> mAvailableApiLevels;

	public:
		RemoteReaderOfferMsg(const QString& pDeviceName, bool pEncrypted, quint16 pPort, const QVector<int>& pAvailableApiLevels);
		virtual ~RemoteReaderOfferMsg() = default;

		const QString& getDeviceName() const;

		bool isEncrypted() const;

		quint16 getPort() const;

		const QVector<int>& getAvailableApiLevels() const;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;

		bool operator==(const RemoteReaderOfferMsg& pOther) const;
		bool isEqual(const RemoteReaderOfferMsg* const pOther) const;
};


class GetApiLevelCmd
	: public RemoteCardNotification
{
	public:
		GetApiLevelCmd();

		virtual ~GetApiLevelCmd() = default;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class SetApiLevelCmd
	: public RemoteCardNotification
{
	private:
		const int mLevel;

	public:
		SetApiLevelCmd(int pLevel);

		virtual ~SetApiLevelCmd() = default;

		int getLevel() const;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class ApiLevelMsg
	: public RemoteCardNotification
{
	private:
		const QString mError;

		const QVector<int> mAvailable;

		const int mCurrent;

	public:
		ApiLevelMsg(const QString& pError, const QVector<int>& pAvailable, int pCurrent);

		virtual ~ApiLevelMsg() = default;

		const QString& getError() const;

		const QVector<int>& getAvailable() const;

		int getCurrent() const;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class GetReaderListCmd
	: public RemoteCardNotification
{
	public:
		GetReaderListCmd();

		virtual ~GetReaderListCmd() = default;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class CardDescription
{
	private:
		const QByteArray mEFCardAccess;

		const int mRetryCounter;

		const bool mPinDeactivated;

		const bool mConnected;

	public:
		CardDescription(const QByteArray& pEFCardAccess = QByteArray(),
				int pRetryCounter = -1,
				bool pPinDeactivated = true,
				bool pConnected = false);

		~CardDescription() = default;

		const QByteArray& getEFCardAccess() const;

		int getRetryCounter() const;

		bool isPinDeactivated() const;

		bool isConnected() const;

		QJsonValue toJson() const;

		bool operator ==(const CardDescription& pOther) const;
};


class ReaderDescription
{
	private:
		class ReaderDescriptionData
			: public QSharedData
		{
			public:
				const QString mName;

				const bool mAttached;

				const ExtendedLengthApduSupportCode mExtendedLength;

				const CardDescription mCard;

				ReaderDescriptionData(const QString& pName = QString(),
						bool pAttached = false,
						ExtendedLengthApduSupportCode pExtendedLength = ExtendedLengthApduSupportCode::INVALID,
						const CardDescription& pCard = CardDescription());

				bool operator==(const ReaderDescriptionData& pOther) const;
		};

		QSharedDataPointer<ReaderDescriptionData> d;

	public:
		ReaderDescription(const QString& pName = QString(),
				bool pAttached = false,
				ExtendedLengthApduSupportCode pExtendedLength = ExtendedLengthApduSupportCode::INVALID,
				const CardDescription& pCard = CardDescription());

		~ReaderDescription() = default;

		const QString& getName() const;

		bool isAttached() const;

		ExtendedLengthApduSupportCode getExtendedLength() const;

		bool hasCard() const;

		const CardDescription& getCard() const;

		QJsonObject toJson() const;

		bool operator ==(const ReaderDescription& pOther) const;
};


class ReaderListMsg
	: public RemoteCardNotification
{
	private:
		const QVector<ReaderDescription> mReaders;

	public:
		ReaderListMsg(const QVector<ReaderDescription>& pReaders);

		virtual ~ReaderListMsg() = default;

		const QVector<ReaderDescription>& getReaders() const;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class ReaderMsg
	: public RemoteCardNotification
{
	private:
		const ReaderDescription mReader;

		const QString mError;

	public:
		ReaderMsg(const ReaderDescription& pReader, const QString& pError);

		virtual ~ReaderMsg() = default;

		const ReaderDescription& getReader() const;

		const QString& getError() const;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class ConnectCmd
	: public RemoteCardNotification
{
	private:
		const QString mReaderName;

	public:
		ConnectCmd(const QString& pReaderName);

		virtual ~ConnectCmd() = default;

		const QString& getReaderName() const;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class DisconnectCmd
	: public RemoteCardNotification
{
	private:
		const QString mReaderName;

	public:
		DisconnectCmd(const QString& pReaderName);

		virtual ~DisconnectCmd() = default;

		const QString& getReaderName() const;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class TransmitCmd
	: public RemoteCardNotification
{
	private:
		const QString mReaderName;

		const QByteArray mCommandApdu;

	public:
		TransmitCmd(const QString& pReaderName, const QByteArray& pCommandApdu);

		virtual ~TransmitCmd() = default;

		const QString& getReaderName() const;

		const QByteArray& getCommandApdu() const;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class TransmitResponseMsg
	: public RemoteCardNotification
{
	private:
		const QString mReaderName;

		const QString mError;

		const QByteArray mResponseApdu;

	public:
		TransmitResponseMsg(const QString& pReaderName, const QString& pError, const QByteArray& pResponseApdu);

		virtual ~TransmitResponseMsg() = default;

		const QString& getReaderName() const;

		const QString& getError() const;

		const QByteArray& getResponseApdu() const;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class UnsupportedMsg
	: public RemoteCardNotification
{
	private:
		const QString mError;

	public:
		UnsupportedMsg(const QString& pError);

		virtual ~UnsupportedMsg() = default;

		const QString& getError() const;

		virtual CardNotificationType getType() const override;

		virtual QJsonDocument toJson() const override;

		virtual void accept(const QSharedPointer<NotificationVisitor>& pVisitor) const override;
};


class NotificationVisitor
{
	public:
		NotificationVisitor() = default;

		virtual ~NotificationVisitor() = default;

		virtual void process(const QSharedPointer<RemoteReaderDiscoverCmd>& pNotification);

		virtual void process(const QSharedPointer<RemoteReaderOfferMsg>& pNotification);

		virtual void process(const QSharedPointer<GetApiLevelCmd>& pNotification);

		virtual void process(const QSharedPointer<SetApiLevelCmd>& pNotification);

		virtual void process(const QSharedPointer<ApiLevelMsg>& pNotification);

		virtual void process(const QSharedPointer<GetReaderListCmd>& pNotification);

		virtual void process(const QSharedPointer<ReaderListMsg>& pNotification);

		virtual void process(const QSharedPointer<ReaderMsg>& pNotification);

		virtual void process(const QSharedPointer<ConnectCmd>& pNotification);

		virtual void process(const QSharedPointer<DisconnectCmd>& pNotification);

		virtual void process(const QSharedPointer<TransmitCmd>& pNotification);

		virtual void process(const QSharedPointer<TransmitResponseMsg>& pNotification);

		virtual void process(const QSharedPointer<UnsupportedMsg>& pNotification);

};


} /* namespace governikus */
