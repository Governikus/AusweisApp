/*!
 * \brief Model implementation for the authentication action.
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "WorkflowModel.h"
#include "context/AuthContext.h"

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

	protected:
		AuthModel();
		~AuthModel() override = default;

	public:
		void resetAuthContext(const QSharedPointer<AuthContext>& pContext = QSharedPointer<AuthContext>());

		[[nodiscard]] const QString& getTransactionInfo() const;
		[[nodiscard]] int getProgressValue() const;
		[[nodiscard]] QString getProgressMessage() const;
		[[nodiscard]] bool getShowChangePinView() const;
		[[nodiscard]] QString getErrorHeader() const;
		[[nodiscard]] QString getErrorText() const;
		[[nodiscard]] QString getStatusCodeString() const;

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
