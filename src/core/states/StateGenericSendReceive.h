/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/AuthContext.h"
#include "paos/PaosMessage.h"
#include "paos/PaosType.h"
#include "paos/invoke/PaosCreator.h"
#include "states/AbstractGenericState.h"

#include <QSharedPointer>
#include <QSslPreSharedKeyAuthenticator>
#include <QVector>

namespace governikus
{

class StateGenericSendReceive
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT

	private:
		const QVector<PaosType> mTypesToReceive;
		QPointer<QNetworkReply> mReply;

		void setMessageId(QSharedPointer<PaosMessage> pPaosMessage);
		void setRemoteMessageId(QSharedPointer<PaosMessage> pPaosMessage);
		void setReceivedMessage(QSharedPointer<PaosMessage> pMessage);
		void reportChannelEstablishmentError(const GlobalStatus::Code pStatusCode);
		bool checkAndSaveCertificate(const QSslCertificate& pCertificate);
		void onSslErrors(const QList<QSslError>& pErrors);
		void onSslHandshakeDone();
		virtual void run();

	protected:
		/**
		 * Creates a new instance of.
		 * \param pUiFactory the UiFactory to use
		 * \param pNewState the entered state when executing this state
		 * \param pTypeToSend the PAOS message type to send
		 * \param pTypesToReceive the expected PAOS message types to receive
		 * \return in case of any error 0 is returned. Otherwise the received PAOS messages position in the list pTypesToReceive plus one is returned.
		 */
		StateGenericSendReceive(QSharedPointer<WorkflowContext> pContext, const QVector<PaosType>& pTypesToReceive);
		StateGenericSendReceive(QSharedPointer<WorkflowContext> pContext, PaosType pTypesToReceive);

		virtual QSharedPointer<PaosCreator> getMessageToSend() = 0;
		virtual void emitStateMachineSignal(int result) = 0;

	private Q_SLOTS:
		void onReplyFinished();
		void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator);
};

class StateSendStartPaos
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	StateSendStartPaos(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>() << PaosType::INITIALIZE_FRAMEWORK << PaosType::DID_LIST
																<< PaosType::DID_AUTHENTICATE_EAC1 << PaosType::STARTPAOS_RESPONSE)
	{
	}


	protected:
		virtual QSharedPointer<PaosCreator> getMessageToSend() override
		{
			return getContext()->getStartPaos().staticCast<PaosCreator>();
		}


		virtual void emitStateMachineSignal(int pResult) override
		{
			if (pResult == 3)
			{
				Q_EMIT fireReceivedDidList();
			}
			else if (pResult == 4)
			{
				Q_EMIT fireReceivedExtractCvcsFromEac1InputType();
			}
			else if (pResult == 5)
			{
				Q_EMIT fireReceivedStartPaosResponse();
			}
		}


	Q_SIGNALS:
		void fireReceivedDidList();
		void fireReceivedExtractCvcsFromEac1InputType();
		void fireReceivedStartPaosResponse();


};

class StateSendInitializeFrameworkResponse
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	StateSendInitializeFrameworkResponse(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>() << PaosType::DID_LIST << PaosType::DID_AUTHENTICATE_EAC1 << PaosType::STARTPAOS_RESPONSE)
	{
	}


	protected:
		virtual QSharedPointer<PaosCreator> getMessageToSend() override
		{
			return getContext()->getInitializeFrameworkResponse().staticCast<PaosCreator>();
		}


		virtual void emitStateMachineSignal(int pResult) override
		{
			if (pResult == 3)
			{
				Q_EMIT fireReceivedExtractCvcsFromEac1InputType();
			}
			else if (pResult == 4)
			{
				Q_EMIT fireReceivedStartPaosResponse();
			}
		}


	Q_SIGNALS:
		void fireReceivedExtractCvcsFromEac1InputType();
		void fireReceivedStartPaosResponse();


};

class StateSendDIDListResponse
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	StateSendDIDListResponse(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>() << PaosType::DID_AUTHENTICATE_EAC1 << PaosType::DISCONNECT << PaosType::STARTPAOS_RESPONSE)
	{
	}


	protected:
		virtual QSharedPointer<PaosCreator> getMessageToSend() override
		{
			return getContext()->getDidListResponse().staticCast<PaosCreator>();
		}


		virtual void emitStateMachineSignal(int pResult) override
		{
			if (pResult == 3)
			{
				Q_EMIT fireReceivedDisconnect();
			}
			else if (pResult == 4)
			{
				Q_EMIT fireReceivedStartPaosResponse();
			}
		}


	Q_SIGNALS:
		void fireReceivedDisconnect();
		void fireReceivedStartPaosResponse();

};

class StateSendDIDAuthenticateResponseEAC1
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	StateSendDIDAuthenticateResponseEAC1(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>() << PaosType::DID_AUTHENTICATE_EAC2 << PaosType::DISCONNECT << PaosType::STARTPAOS_RESPONSE)
	{
	}


	protected:
		virtual QSharedPointer<PaosCreator> getMessageToSend() override
		{
			return getContext()->getDidAuthenticateResponseEac1().staticCast<PaosCreator>();
		}


		virtual void emitStateMachineSignal(int pResult) override
		{
			if (pResult == 3)
			{
				Q_EMIT fireReceivedDisconnect();
			}
			else if (pResult == 4)
			{
				Q_EMIT fireReceivedStartPaosResponse();
			}
		}


	Q_SIGNALS:
		void fireReceivedDisconnect();
		void fireReceivedStartPaosResponse();

};

class StateSendDIDAuthenticateResponseEACAdditionalInputType
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	StateSendDIDAuthenticateResponseEACAdditionalInputType(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>() << PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE << PaosType::STARTPAOS_RESPONSE)
	{
	}


	protected:
		virtual QSharedPointer<PaosCreator> getMessageToSend() override
		{
			return getContext()->getDidAuthenticateResponseEacAdditionalInputType().staticCast<PaosCreator>();
		}


		virtual void emitStateMachineSignal(int pResult) override
		{
			if (pResult == 3)
			{
				Q_EMIT fireReceivedStartPaosResponse();
			}
		}


	Q_SIGNALS:
		void fireReceivedStartPaosResponse();

};

class StateSendDIDAuthenticateResponseEAC2
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	StateSendDIDAuthenticateResponseEAC2(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>() << PaosType::TRANSMIT << PaosType::DISCONNECT << PaosType::STARTPAOS_RESPONSE)
	{
	}


	protected:
		virtual QSharedPointer<PaosCreator> getMessageToSend() override
		{
			return getContext()->getDidAuthenticateResponseEac2().staticCast<PaosCreator>();
		}


		virtual void emitStateMachineSignal(int pResult) override
		{
			if (pResult == 3)
			{
				Q_EMIT fireReceivedDisconnect();
			}
			else if (pResult == 4)
			{
				Q_EMIT fireReceivedStartPaosResponse();
			}
		}


	Q_SIGNALS:
		void fireReceivedDisconnect();
		void fireReceivedStartPaosResponse();

};

class StateSendTransmitResponse
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	StateSendTransmitResponse(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>() << PaosType::TRANSMIT << PaosType::DISCONNECT << PaosType::STARTPAOS_RESPONSE)
	{
	}


	protected:
		virtual QSharedPointer<PaosCreator> getMessageToSend() override
		{
			return getContext()->getTransmitResponses().last().staticCast<PaosCreator>();
		}


		virtual void emitStateMachineSignal(int pResult) override
		{
			if (pResult == 3)
			{
				Q_EMIT fireReceivedDisconnect();
			}
			else if (pResult == 4)
			{
				Q_EMIT fireReceivedStartPaosResponse();
			}
		}


	Q_SIGNALS:
		void fireReceivedDisconnect();
		void fireReceivedStartPaosResponse();


};

class StateSendDisconnectResponse
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	StateSendDisconnectResponse(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, PaosType::STARTPAOS_RESPONSE)
	{
	}


	protected:
		virtual QSharedPointer<PaosCreator> getMessageToSend() override
		{
			return getContext()->getDisconnectResponse();
		}


		virtual void emitStateMachineSignal(int pResult) override
		{
			Q_UNUSED(pResult);
		}


};

} /* namespace governikus */
