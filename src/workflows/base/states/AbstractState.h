/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Base class for all states taken by the state machine.
 */

#pragma once

#include "FailureCode.h"
#include "context/WorkflowContext.h"

#include <QSharedPointer>
#include <QState>


namespace governikus
{

class AbstractState
	: public QState
{
	Q_OBJECT

	private:
		const QSharedPointer<WorkflowContext> mContext;
		bool mAbortOnCardRemoved;
		bool mKeepCardConnectionAlive;

		virtual void run() = 0;
		[[nodiscard]] bool isStartStopEnabled() const;

	protected:
		QVector<QMetaObject::Connection> mConnections;

		explicit AbstractState(const QSharedPointer<WorkflowContext>& pContext);

		void setAbortOnCardRemoved();
		void setKeepCardConnectionAlive();

		void onEntry(QEvent* pEvent) override;
		void onExit(QEvent* pEvent) override;

		void clearConnections();
		bool isCancellationByUser();
		void updateStatus(const GlobalStatus& pStatus);
		void updateStartPaosResult(const ECardApiResult& pStartPaosResult);

		void stopNfcScanIfNecessary(const QString& pError = QString());

	public:
		static const char* const cFORCE_START_STOP_SCAN;
		static QString getClassName(const char* const pName);

		template<typename T>
		[[nodiscard]] static QString getClassName()
		{
			return getClassName(T::staticMetaObject.className());
		}


		template<typename T>
		static bool isState(const QString& pState)
		{
			return pState == getClassName<T>();
		}


		~AbstractState() override = default;

		[[nodiscard]] QString getStateName() const;
		void setStateName(const QString& pName);

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
