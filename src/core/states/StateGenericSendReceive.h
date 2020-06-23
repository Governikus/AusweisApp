/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/AuthContext.h"
#include "GenericContextContainer.h"
#include "paos/invoke/PaosCreator.h"
#include "paos/PaosMessage.h"
#include "paos/PaosType.h"

#include <QSharedPointer>
#include <QSslPreSharedKeyAuthenticator>
#include <QVector>

class test_StateGenericSendReceive;

namespace governikus
{

class StateGenericSendReceive
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT

	private:
		friend class ::test_StateGenericSendReceive;
		const QVector<PaosType> mTypesToReceive;
		QPointer<QNetworkReply> mReply;

		void setReceivedMessage(const QSharedPointer<PaosMessage>& pMessage);
		GlobalStatus::Code checkAndSaveCertificate(const QSslCertificate& pCertificate);
		void onSslErrors(const QList<QSslError>& pErrors);
		void onSslHandshakeDone();
		virtual void run() override;

	protected:
		explicit StateGenericSendReceive(const QSharedPointer<WorkflowContext>& pContext, const QVector<PaosType>& pTypesToReceive, bool pConnectOnCardRemoved = true);
		explicit StateGenericSendReceive(const QSharedPointer<WorkflowContext>& pContext, PaosType pTypesToReceive);

		virtual QSharedPointer<PaosMessage> getAsMessage() = 0;
		virtual QSharedPointer<ResponseType> getAsResponse() = 0;
		virtual QSharedPointer<PaosCreator> getAsCreator() = 0;
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

	explicit StateSendStartPaos(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>
				{
					PaosType::INITIALIZE_FRAMEWORK, PaosType::DID_LIST,
					PaosType::DID_AUTHENTICATE_EAC1, PaosType::STARTPAOS_RESPONSE
				}, false)
	{
	}

	protected:
		virtual QSharedPointer<PaosMessage> getAsMessage() override
		{
			return getContext()->getStartPaos();
		}


		virtual QSharedPointer<ResponseType> getAsResponse() override
		{
			return QSharedPointer<ResponseType>();
		}


		virtual QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getStartPaos();
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

	explicit StateSendInitializeFrameworkResponse(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>
				{
					PaosType::DID_LIST, PaosType::DID_AUTHENTICATE_EAC1, PaosType::STARTPAOS_RESPONSE
				}, false)
	{
	}

	protected:
		virtual QSharedPointer<PaosMessage> getAsMessage() override
		{
			return getContext()->getInitializeFrameworkResponse();
		}


		virtual QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getInitializeFrameworkResponse();
		}


		virtual QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getInitializeFrameworkResponse();
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

	explicit StateSendDIDListResponse(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>
				{
					PaosType::DID_AUTHENTICATE_EAC1, PaosType::DISCONNECT, PaosType::STARTPAOS_RESPONSE
				}, false)
	{
	}

	protected:
		virtual QSharedPointer<PaosMessage> getAsMessage() override
		{
			return getContext()->getDidListResponse();
		}


		virtual QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getDidListResponse();
		}


		virtual QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getDidListResponse();
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

	explicit StateSendDIDAuthenticateResponseEAC1(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>
				{
					PaosType::DID_AUTHENTICATE_EAC2, PaosType::DISCONNECT, PaosType::STARTPAOS_RESPONSE
				})
	{
	}

	protected:
		virtual QSharedPointer<PaosMessage> getAsMessage() override
		{
			return getContext()->getDidAuthenticateResponseEac1();
		}


		virtual QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getDidAuthenticateResponseEac1();
		}


		virtual QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getDidAuthenticateResponseEac1();
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

	explicit StateSendDIDAuthenticateResponseEACAdditionalInputType(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>
				{
					PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE, PaosType::STARTPAOS_RESPONSE
				})
	{
	}

	protected:
		virtual QSharedPointer<PaosMessage> getAsMessage() override
		{
			return getContext()->getDidAuthenticateResponseEacAdditionalInputType();
		}


		virtual QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getDidAuthenticateResponseEacAdditionalInputType();
		}


		virtual QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getDidAuthenticateResponseEacAdditionalInputType();
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

	explicit StateSendDIDAuthenticateResponseEAC2(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>
				{
					PaosType::TRANSMIT, PaosType::DISCONNECT, PaosType::STARTPAOS_RESPONSE
				})
	{
	}

	protected:
		virtual QSharedPointer<PaosMessage> getAsMessage() override
		{
			return getContext()->getDidAuthenticateResponseEac2();
		}


		virtual QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getDidAuthenticateResponseEac2();
		}


		virtual QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getDidAuthenticateResponseEac2();
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

	explicit StateSendTransmitResponse(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, QVector<PaosType>
				{
					PaosType::TRANSMIT, PaosType::DISCONNECT, PaosType::STARTPAOS_RESPONSE
				})
	{
	}

	protected:
		virtual QSharedPointer<PaosMessage> getAsMessage() override
		{
			return getContext()->getTransmitResponses().constLast();
		}


		virtual QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getTransmitResponses().constLast();
		}


		virtual QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getTransmitResponses().constLast();
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

	explicit StateSendDisconnectResponse(const QSharedPointer<WorkflowContext>& pContext)
		: StateGenericSendReceive(pContext, PaosType::STARTPAOS_RESPONSE)
	{
	}

	protected:
		virtual QSharedPointer<PaosMessage> getAsMessage() override
		{
			return getContext()->getDisconnectResponse();
		}


		virtual QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getDisconnectResponse();
		}


		virtual QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getDisconnectResponse();
		}


		virtual void emitStateMachineSignal(int pResult) override
		{
			Q_UNUSED(pResult)
		}


};

} // namespace governikus
