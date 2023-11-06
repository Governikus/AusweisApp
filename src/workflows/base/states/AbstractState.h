/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Base class for all states taken by the state machine.
 */

#pragma once

#include "FailureCode.h"
#include "StateBuilder.h"
#include "context/WorkflowContext.h"

#include <QSharedPointer>
#include <QState>


class test_StateChangePinIfd;
class test_StateChangeSmartPin;
class test_StateEstablishPaceChannelIfd;
class test_StateProcessIfdMessages;
class test_StateChangePin;
class test_StateDestroyPace;
class test_StateDidAuthenticateEac1;
class test_StateDidAuthenticateEac2;
class test_StateGetTcToken;
class test_StateTransmit;


namespace governikus
{

class AbstractState
	: public QState
{
	Q_OBJECT
	friend class ::test_StateChangePinIfd;
	friend class ::test_StateChangeSmartPin;
	friend class ::test_StateEstablishPaceChannelIfd;
	friend class ::test_StateProcessIfdMessages;
	friend class ::test_StateChangePin;
	friend class ::test_StateDestroyPace;
	friend class ::test_StateDidAuthenticateEac1;
	friend class ::test_StateDidAuthenticateEac2;
	friend class ::test_StateGetTcToken;
	friend class ::test_StateTransmit;
	friend class ::test_StateChangeSmartPin;

	private:
		const QSharedPointer<WorkflowContext> mContext;
		QVector<QMetaObject::Connection> mConnections;
		bool mAbortOnCardRemoved;
		bool mKeepCardConnectionAlive;

		virtual void run() = 0;
		[[nodiscard]] bool isStartStopEnabled() const;

	protected:
		explicit AbstractState(const QSharedPointer<WorkflowContext>& pContext);

		void setAbortOnCardRemoved();
		void setKeepCardConnectionAlive();

		void onEntry(QEvent* pEvent) override;
		void onExit(QEvent* pEvent) override;

		void operator<<(const QMetaObject::Connection& connection);

		void clearConnections();
		bool isCancellationByUser() const;
		void updateStatus(const GlobalStatus& pStatus);
		void updateStartPaosResult(const ECardApiResult& pStartPaosResult);

		void stopNfcScanIfNecessary(const QString& pError = QString()) const;

	public:
		static const char* const cFORCE_START_STOP_SCAN;

		~AbstractState() override = default;

		[[nodiscard]] QString getStateName() const;

	Q_SIGNALS:
		void fireContinue();
		void fireAbort(const FailureCode& pFailure);

	private Q_SLOTS:
		void onAbort(const FailureCode& pFailure) const;

	public Q_SLOTS:
		void onStateApprovedChanged(bool pApproved);
		virtual void onUserCancelled();
		void onCardRemoved(const ReaderInfo& pInfo);
};

} // namespace governikus
