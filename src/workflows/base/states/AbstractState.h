/*!
 * \brief Base class for all states taken by the state machine.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

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
		const bool mConnectOnCardRemoved;

		virtual void run() = 0;
		[[nodiscard]] bool isStartStopEnabled() const;

	protected:
		QVector<QMetaObject::Connection> mConnections;

		explicit AbstractState(const QSharedPointer<WorkflowContext>& pContext, bool pConnectOnCardRemoved = true);

		void onExit(QEvent* pEvent) override;

		void clearConnections();
		bool isCancellationByUser();
		void updateStatus(const GlobalStatus& pStatus);
		void updateStartPaosResult(const ECardApiResult& pStartPaosResult);

		void startScanIfNecessary();
		void stopScanIfNecessary(const QString& pError = QString());

	public:
		static const char* const cFORCE_START_STOP_SCAN;
		static QString getClassName(const char* const pName);

		template<typename T>
		static bool isState(const QString& pState)
		{
			return pState == getClassName(T::staticMetaObject.className());
		}


		~AbstractState() override;

		void onEntry(QEvent* pEvent) override;

		[[nodiscard]] QString getStateName() const;
		void setStateName(const QString& pName);

	Q_SIGNALS:
		void fireContinue();
		void fireAbort();

	public Q_SLOTS:
		void onStateApprovedChanged(bool pApproved);
		virtual void onUserCancelled();
		void onCardRemoved(const ReaderInfo& pInfo);
};

} // namespace governikus
