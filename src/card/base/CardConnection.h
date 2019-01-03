/*!
 * \brief Contains a card connection object
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/Chat.h"
#include "asn1/CVCertificate.h"
#include "asn1/CVCertificateChain.h"
#include "command/BaseCardCommand.h"
#include "CardConnectionWorker.h"
#include "Commands.h"
#include "InputAPDUInfo.h"
#include "ReaderInfo.h"
#include "SmartCardDefinitions.h"

#include "command/DestroyPaceChannelCommand.h"
#include "command/EstablishPaceChannelCommand.h"
#include "command/SetEidPinCommand.h"

#include "command/DidAuthenticateEAC1Command.h"
#include "command/DidAuthenticateEAC2Command.h"
#include "command/TransmitCommand.h"
#include "command/UnblockPinCommand.h"
#include "command/UpdateRetryCounterCommand.h"

#include <QByteArray>

class test_WorkflowContext;
class test_SelfAuthModel;
class test_CardConnection;
class test_StateEstablishPaceChannel;


namespace governikus
{

/*!
 * This class represents a connection to a smart card.
 */
class CardConnection
	: public QObject
{
	private:
		friend class ::test_WorkflowContext;
		friend class ::test_SelfAuthModel;
		friend class ::test_CardConnection;
		friend class ::test_StateEstablishPaceChannel;

		Q_OBJECT

		/*!
		 * The connection worker talks to the Card held by the Reader.
		 */
		QSharedPointer<CardConnectionWorker> mCardConnectionWorker;
		ReaderInfo mReaderInfo;

		bool mPaceCanSuccessful;
		bool mPacePinSuccessful;

		TransmitCommand* createTransmitCommand(const QVector<InputAPDUInfo>& pInputApduInfos, const QString& pSlotHandle);
		UpdateRetryCounterCommand* createUpdateRetryCounterCommand();
		UnblockPinCommand* createUnblockPinCommand(const QString& pPuk);

		EstablishPaceChannelCommand* createEstablishPaceChannelCommand(PacePasswordId pPacePasswordId, const QString& pPacePassword, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription);
		SetEidPinCommand* createSetEidPinCommand(const QString& pNewPin, quint8 pTimeoutSeconds);
		DestroyPaceChannelCommand* createDestroyPaceChannelCommand();

		DidAuthenticateEAC1Command* createDidAuthenticateEAC1Command();
		DidAuthenticateEAC2Command* createDidAuthenticateEAC2Command(const CVCertificateChain& pCvcChain,
				const QString& pEphemeralPublicKeyAsHex,
				const QString& pSignatureAsHex,
				const QByteArray& pAuthenticatedAuxiliaryDataAsBinary);

		template<typename T>
		QMetaObject::Connection call(BaseCardCommand* pCommand, const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc)
		{
			pCommand->moveToThread(mCardConnectionWorker->thread());

			QMetaObject::Connection resultConnection = connect(pCommand, &BaseCardCommand::commandDone, pReceiver, pFunc, Qt::UniqueConnection);

			if (resultConnection)
			{
				pCommand->run();
			}
			else
			{
				qCritical() << "Cannot invoke card command:" << pCommand->metaObject()->className();
				pCommand->deleteLater();
			}

			return resultConnection;
		}


	private Q_SLOTS:
		void onReaderInfoChanged(const ReaderInfo& pReaderInfo);

	protected:
		CardConnection();

	public:
		CardConnection(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

		/*!
		 * Destroys the CardConnection and disconnects from the card.
		 */
		virtual ~CardConnection() = default;

		/*!
		 * This method returns a stored copy of the reader info object. So calling this method any
		 * time will never result in dead locks.
		 *
		 * (In contrast ReaderManager::getReaderInfo(...) calls the worker thread and may be blocked.)
		 */
		virtual const ReaderInfo& getReaderInfo();

		bool getPaceCanSuccessful() const;
		bool getPacePinSuccessful() const;

		bool stopSecureMessaging();

		template<typename T>
		QMetaObject::Connection callDidAuthenticateEAC1Command(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc)
		{
			auto command = createDidAuthenticateEAC1Command();
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callDidAuthenticateEAC2Command(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc,
			const CVCertificateChain& pCvcChain,
			const QString& pEphemeralPublicKeyAsHex,
			const QString& pSignatureAsHex,
			const QByteArray& pAuthenticatedAuxiliaryDataAsBinary)
		{
			auto command = createDidAuthenticateEAC2Command(pCvcChain, pEphemeralPublicKeyAsHex, pSignatureAsHex, pAuthenticatedAuxiliaryDataAsBinary);
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callUnblockPinCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc,
			const QString& pPuk)
		{
			auto command = createUnblockPinCommand(pPuk);
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callEstablishPaceChannelCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc,
			PacePasswordId pPacePasswordId, const QString& pPacePassword, const QByteArray& pEffectiveChat = QByteArray(), const QByteArray& pCertificateDescription = QByteArray())
		{
			EstablishPaceChannelCommand* command = createEstablishPaceChannelCommand(pPacePasswordId, pPacePassword, pEffectiveChat, pCertificateDescription);

			if (pPacePasswordId == PacePasswordId::PACE_CAN)
			{
				connect(command, &BaseCardCommand::commandDone, this, [this](QSharedPointer<BaseCardCommand> pCommand){
							mPaceCanSuccessful = pCommand->getReturnCode() == CardReturnCode::OK;
						});
			}
			else if (pPacePasswordId == PacePasswordId::PACE_PIN)
			{
				connect(command, &BaseCardCommand::commandDone, this, [this](QSharedPointer<BaseCardCommand> pCommand){
							mPacePinSuccessful = pCommand->getReturnCode() == CardReturnCode::OK;
						});
			}

			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callSetEidPinCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc,
			const QString& pNewPin,
			quint8 pTimeoutSeconds = 60)
		{
			auto command = createSetEidPinCommand(pNewPin, pTimeoutSeconds);
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callDestroyPaceChannelCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc)
		{
			auto command = createDestroyPaceChannelCommand();
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callTransmitCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc,
			const QVector<InputAPDUInfo>& pInputApduInfos, const QString& pSlotHandle = QString())
		{
			auto command = createTransmitCommand(pInputApduInfos, pSlotHandle);
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callUpdateRetryCounterCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc)
		{
			auto command = createUpdateRetryCounterCommand();
			return call(command, pReceiver, pFunc);
		}


	Q_SIGNALS:
		void fireReaderInfoChanged(const ReaderInfo& pReaderInfo);
};

} // namespace governikus
