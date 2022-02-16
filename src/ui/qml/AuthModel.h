/*!
 * \brief Model implementation for the authentication action.
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "Env.h"
#include "WorkflowModel.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QString>

namespace governikus
{

class AuthModel
	: public WorkflowModel
{
	Q_OBJECT
	friend class Env;

	Q_PROPERTY(QString transactionInfo READ getTransactionInfo NOTIFY fireTransactionInfoChanged)
	Q_PROPERTY(int progressValue READ getProgressValue NOTIFY fireProgressChanged)
	Q_PROPERTY(QString progressMessage READ getProgressMessage NOTIFY fireProgressChanged)
	Q_PROPERTY(bool showChangePinView READ getShowChangePinView NOTIFY fireShowChangePinViewChanged)
	Q_PROPERTY(QString errorHeader READ getErrorHeader NOTIFY fireResultChanged)
	Q_PROPERTY(QString errorText READ getErrorText NOTIFY fireResultChanged)
	Q_PROPERTY(QString statusCodeString READ getStatusCodeString NOTIFY fireResultChanged)

	private:
		QSharedPointer<AuthContext> mContext;
		QString mTransactionInfo;

		AuthModel();
		~AuthModel() override = default;

	public:
		void resetAuthContext(const QSharedPointer<AuthContext>& pContext = QSharedPointer<AuthContext>());

		const QString& getTransactionInfo() const;
		int getProgressValue() const;
		QString getProgressMessage() const;
		bool getShowChangePinView() const;
		QString getErrorHeader() const;
		QString getErrorText() const;
		QString getStatusCodeString() const;

		Q_INVOKABLE void cancelWorkflowToChangePin();
		Q_INVOKABLE void requestTransportPinChange();

	private Q_SLOTS:
		void onDidAuthenticateEac1Changed();

	Q_SIGNALS:
		void fireShowChangePinViewChanged();
		void fireTransactionInfoChanged();
		void fireProgressChanged();
};


} // namespace governikus
