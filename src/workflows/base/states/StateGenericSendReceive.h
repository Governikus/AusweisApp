/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"
#include "paos/PaosMessage.h"
#include "paos/PaosType.h"
#include "paos/invoke/PaosCreator.h"

#include <QList>
#include <QSharedPointer>
#include <QSslPreSharedKeyAuthenticator>
#include <optional>

class test_StateGenericSendReceive;

namespace governikus
{

class StateGenericSendReceive
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class ::test_StateGenericSendReceive;

	private:
		const PaosType mExpectedResponseType;
		const QList<PaosType> mOtherResponseTypes;
		const bool mPersonalization;
		QSharedPointer<QNetworkReply> mReply;

		void logRawData(const QByteArray& pMessage);
		void setReceivedMessage(const QSharedPointer<PaosMessage>& pMessage) const;
		std::optional<FailureCode> checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration);
		void onSslErrors(const QList<QSslError>& pErrors);
		void onSslHandshakeDone();
		void run() override;

	protected:
		explicit StateGenericSendReceive(
			const QSharedPointer<WorkflowContext>& pContext,
			PaosType pExpectedResponseType,
			const QList<PaosType>& pOtherResponseTypes = {},
			bool pPersonalization = false);

		virtual QSharedPointer<ResponseType> getAsResponse() = 0;
		virtual QSharedPointer<PaosCreator> getAsCreator() = 0;
		virtual void emitStateMachineSignal(PaosType pResponseType);

	private Q_SLOTS:
		void onReplyFinished();
		void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator) const;

	public:
		void onExit(QEvent* pEvent) override;

	Q_SIGNALS:
		void fireReceivedStartPaosResponse();
};

class StateSendStartPaos
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateSendStartPaos(const QSharedPointer<WorkflowContext>& pContext)
			: StateGenericSendReceive(pContext,
					PaosType::INITIALIZE_FRAMEWORK,
					{
						PaosType::DID_AUTHENTICATE_EAC1
					})
		{
		}

	protected:
		QSharedPointer<ResponseType> getAsResponse() override
		{
			return QSharedPointer<ResponseType>();
		}


		QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getStartPaos();
		}


		void emitStateMachineSignal(PaosType pResponseType) override
		{
			if (pResponseType == PaosType::DID_AUTHENTICATE_EAC1)
			{
				Q_EMIT fireReceivedExtractCvcsFromEac1InputType();
			}
		}

	Q_SIGNALS:
		void fireReceivedExtractCvcsFromEac1InputType();


};

class StateSendInitializeFrameworkResponse
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateSendInitializeFrameworkResponse(const QSharedPointer<WorkflowContext>& pContext)
			: StateGenericSendReceive(pContext,
					PaosType::DID_AUTHENTICATE_EAC1)
		{
		}

	protected:
		QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getInitializeFrameworkResponse();
		}


		QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getInitializeFrameworkResponse();
		}


};

class StateSendDIDAuthenticateResponseEAC1
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateSendDIDAuthenticateResponseEAC1(const QSharedPointer<WorkflowContext>& pContext)
			: StateGenericSendReceive(pContext,
					PaosType::DID_AUTHENTICATE_EAC2)
		{
			setAbortOnCardRemoved();
		}

	protected:
		QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getDidAuthenticateResponseEac1();
		}


		QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getDidAuthenticateResponseEac1();
		}


};

class StateSendDIDAuthenticateResponseEACAdditionalInputType
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateSendDIDAuthenticateResponseEACAdditionalInputType(const QSharedPointer<WorkflowContext>& pContext)
			: StateGenericSendReceive(pContext,
					PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE)
		{
			setAbortOnCardRemoved();
		}

	protected:
		QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getDidAuthenticateResponseEacAdditionalInputType();
		}


		QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getDidAuthenticateResponseEacAdditionalInputType();
		}


};

class StateSendDIDAuthenticateResponseEAC2
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateSendDIDAuthenticateResponseEAC2(const QSharedPointer<WorkflowContext>& pContext)
			: StateGenericSendReceive(pContext,
					PaosType::TRANSMIT)
		{
			setAbortOnCardRemoved();
		}

	protected:
		QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getDidAuthenticateResponseEac2();
		}


		QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getDidAuthenticateResponseEac2();
		}


};

class StateSendTransmitResponse
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateSendTransmitResponse(const QSharedPointer<WorkflowContext>& pContext)
			: StateGenericSendReceive(pContext,
					PaosType::TRANSMIT)
		{
			setAbortOnCardRemoved();
		}

	protected:
		QSharedPointer<ResponseType> getAsResponse() override
		{
			return getContext()->getTransmitResponse();
		}


		QSharedPointer<PaosCreator> getAsCreator() override
		{
			return getContext()->getTransmitResponse();
		}


};

} // namespace governikus
