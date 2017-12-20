/*!
 * \brief Model implementation for the authentication action.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "WorkflowModel.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>


namespace governikus
{

class AuthContext;

class AuthModel
	: public WorkflowModel
{
	Q_OBJECT

	Q_PROPERTY(QString transactionInfo READ getTransactionInfo NOTIFY fireTransactionInfoChanged)

	QSharedPointer<AuthContext> mContext;
	QString mTransactionInfo;

	public:
		AuthModel(QObject* pParent = nullptr);
		virtual ~AuthModel() override;

		void resetContext(const QSharedPointer<AuthContext>& pContext = QSharedPointer<AuthContext>());

		const QString& getTransactionInfo() const;

	public Q_SLOTS:
		void onDidAuthenticateEac1Changed();

	Q_SIGNALS:
		void fireTransactionInfoChanged();
};


} /* namespace governikus */
