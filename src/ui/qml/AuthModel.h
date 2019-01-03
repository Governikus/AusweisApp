/*!
 * \brief Model implementation for the authentication action.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "WorkflowModel.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QString>

class test_AuthModel;

namespace governikus
{

class AuthModel
	: public WorkflowModel
{
	Q_OBJECT

	Q_PROPERTY(QString transactionInfo READ getTransactionInfo NOTIFY fireTransactionInfoChanged)

	private:
		friend class ::test_AuthModel;
		QSharedPointer<AuthContext> mContext;
		QString mTransactionInfo;

	protected:
		AuthModel();
		~AuthModel() override = default;

	public:
		void resetContext(const QSharedPointer<AuthContext>& pContext = QSharedPointer<AuthContext>());

		const QString& getTransactionInfo() const;

		static AuthModel& getInstance();

	public Q_SLOTS:
		void onDidAuthenticateEac1Changed();

	Q_SIGNALS:
		void fireTransactionInfoChanged();
};


} // namespace governikus
