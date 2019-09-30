/*!
 * \brief Base class for all states taken by the state machine.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
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
	Q_DISABLE_COPY(AbstractState)

	template<typename ModelClass> friend class AbstractGenericState;

	private:
		const QSharedPointer<WorkflowContext> mContext;
		const bool mConnectOnCardRemoved;

		virtual void run() = 0;

	protected:
		QVector<QMetaObject::Connection> mConnections;

		explicit AbstractState(const QSharedPointer<WorkflowContext>& pContext, bool pConnectOnCardRemoved = true);

		void onExit(QEvent* pEvent) override;

		void clearConnections();
		bool isCancellationByUser();
		void updateStatus(const GlobalStatus& pStatus);
		void updateStartPaosResult(const ECardApiResult& pStartPaosResult);

	public:
		static QString getClassName(const char* const pName);

		template<typename T>
		static bool isState(const QString& pState)
		{
			return pState == getClassName(T::staticMetaObject.className());
		}


		virtual ~AbstractState() override;

		void onEntry(QEvent* pEvent) override;

		QString getStateName() const;
		void setStateName(const QString& pName);

	Q_SIGNALS:
		void fireContinue();
		void fireAbort();

	public Q_SLOTS:
		void onStateApprovedChanged();
		virtual void onUserCancelled();
		void onCardRemoved(const QString& pReaderName);
};

} // namespace governikus
