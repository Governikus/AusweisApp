/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnectionWorker.h"
#include "InputAPDUInfo.h"
#include "ReaderInfo.h"
#include "SmartCardDefinitions.h"
#include "asn1/CVCertificateChain.h"
#include "command/BaseCardCommand.h"

#include "command/DestroyPaceChannelCommand.h"
#include "command/EstablishPaceChannelCommand.h"
#include "command/SetEidPinCommand.h"

#include "command/DidAuthenticateEAC1Command.h"
#include "command/DidAuthenticateEAC2Command.h"
#include "command/ResetRetryCounterCommand.h"
#include "command/TransmitCommand.h"
#include "command/UpdateRetryCounterCommand.h"

#include <QByteArray>
#include <QLoggingCategory>


class test_CardConnection;


namespace governikus
{
class CardConnection
	: public QObject
{
	Q_OBJECT
	friend class MockCardConnection;
	friend class ::test_CardConnection;

	private:
		static const QLoggingCategory& getLoggingCategory();

		/*!
		 * The connection worker talks to the Card held by the Reader.
		 */
		QSharedPointer<CardConnectionWorker> mCardConnectionWorker;
		ReaderInfo mReaderInfo;

		bool mPaceCanSuccessful;
		bool mPacePinSuccessful;

		TransmitCommand* createTransmitCommand(const QList<InputAPDUInfo>& pInputApduInfos, const QString& pSlotHandle);
		UpdateRetryCounterCommand* createUpdateRetryCounterCommand(const QString& pSlotHandle);
		ResetRetryCounterCommand* createResetRetryCounterCommand();

		EstablishPaceChannelCommand* createEstablishPaceChannelCommand(PacePasswordId pPacePasswordId, const QByteArray& pPacePassword, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription);
		SetEidPinCommand* createSetEidPinCommand(const QByteArray& pNewPin, quint8 pTimeoutSeconds);
		DestroyPaceChannelCommand* createDestroyPaceChannelCommand(const QString& pSlotHandle);

		DidAuthenticateEAC1Command* createDidAuthenticateEAC1Command();
		DidAuthenticateEAC2Command* createDidAuthenticateEAC2Command(const CVCertificateChain& pCvcChain,
				const QByteArray& pEphemeralPublicKeyAsHex,
				const QByteArray& pSignatureAsHex,
				const QByteArray& pAuthenticatedAuxiliaryDataAsBinary,
				const QByteArray& pPin);

		template<typename T>
		QMetaObject::Connection call(BaseCardCommand* pCommand, const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc)
		{
			pCommand->moveToThread(mCardConnectionWorker->thread());

			QMetaObject::Connection resultConnection = connect(pCommand, &BaseCardCommand::commandDone, pReceiver, pFunc);

			if (resultConnection)
			{
				pCommand->run();
			}
			else
			{
				qCCritical(getLoggingCategory()) << "Cannot invoke card command:" << pCommand->metaObject()->className();
				pCommand->deleteLater();
			}

			return resultConnection;
		}

	private Q_SLOTS:
		void onReaderInfoChanged(const ReaderInfo& pReaderInfo);

	protected:
		CardConnection();

	public:
		explicit CardConnection(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

		/*!
		 * Destroys the CardConnection and disconnects from the card.
		 */
		~CardConnection() override = default;

		/*!
		 * This method returns a stored copy of the reader info object. So calling this method any
		 * time will never result in dead locks.
		 *
		 * (In contrast ReaderManager::getReaderInfo(...) calls the worker thread and may be blocked.)
		 */
		virtual const ReaderInfo& getReaderInfo();

		[[nodiscard]] bool getPaceCanSuccessful() const;
		[[nodiscard]] bool getPacePinSuccessful() const;

		void setKeepAlive(bool pEnabled);
		void setProgressMessage(const QString& pMessage, int pProgress = -1);
		void setErrorMessage(const QString& pMessage);

		template<typename T>
		QMetaObject::Connection callDidAuthenticateEAC1Command(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc)
		{
			auto command = createDidAuthenticateEAC1Command();
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callDidAuthenticateEAC2Command(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc,
			const CVCertificateChain& pCvcChain,
			const QByteArray& pEphemeralPublicKeyAsHex,
			const QByteArray& pSignatureAsHex,
			const QByteArray& pAuthenticatedAuxiliaryDataAsBinary,
			const QByteArray& pPin)
		{
			auto command = createDidAuthenticateEAC2Command(pCvcChain, pEphemeralPublicKeyAsHex, pSignatureAsHex, pAuthenticatedAuxiliaryDataAsBinary, pPin);
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callEstablishPaceChannelCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc,
			PacePasswordId pPacePasswordId, const QByteArray& pPacePassword, const QByteArray& pEffectiveChat = QByteArray(), const QByteArray& pCertificateDescription = QByteArray())
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
							if (!mPacePinSuccessful)
							{
								mPaceCanSuccessful = false;
							}
						});
			}

			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callSetEidPinCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc,
			const QByteArray& pNewPin,
			quint8 pTimeoutSeconds = Card::DEFAULT_PINPAD_TIMEOUT)
		{
			auto command = createSetEidPinCommand(pNewPin, pTimeoutSeconds);
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callDestroyPaceChannelCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc,
			const QString& pSlotHandle = QString())
		{
			auto command = createDestroyPaceChannelCommand(pSlotHandle);
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callTransmitCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc,
			const QList<InputAPDUInfo>& pInputApduInfos, const QString& pSlotHandle = QString())
		{
			auto command = createTransmitCommand(pInputApduInfos, pSlotHandle);
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callUpdateRetryCounterCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc,
			const QString& pSlotHandle = QString())
		{
			auto command = createUpdateRetryCounterCommand(pSlotHandle);
			return call(command, pReceiver, pFunc);
		}


		template<typename T>
		QMetaObject::Connection callResetRetryCounterCommand(const typename QtPrivate::FunctionPointer<T>::Object* pReceiver, T pFunc)
		{
			auto command = createResetRetryCounterCommand();
			return call(command, pReceiver, pFunc);
		}

	Q_SIGNALS:
		void fireReaderInfoChanged(const ReaderInfo& pReaderInfo);
};

} // namespace governikus
