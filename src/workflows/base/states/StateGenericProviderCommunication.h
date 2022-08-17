/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "GlobalStatus.h"
#include "context/AuthContext.h"

#include <QByteArray>
#include <QList>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QSslConfiguration>
#include <QSslError>


class test_StateGenericProviderCommunication;
class test_StateGetSelfAuthenticationData;
class test_StatePreparePersonalization;
class test_StateGetSessionId;
class test_StateGetChallenge;


namespace governikus
{

class StateGenericProviderCommunication
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class ::test_StateGenericProviderCommunication;
	friend class ::test_StateGetSelfAuthenticationData;
	friend class ::test_StatePreparePersonalization;
	friend class ::test_StateGetSessionId;
	friend class ::test_StateGetChallenge;

	protected:
		QSharedPointer<QNetworkReply> mReply;

		explicit StateGenericProviderCommunication(const QSharedPointer<WorkflowContext>& pContext);

		virtual void handleNetworkReply(const QByteArray& pContent) = 0;
		virtual bool isLoggingAllowed();

	private:
		virtual void setProgress() const;
		[[nodiscard]] virtual QUrl getRequestUrl() const = 0;
		[[nodiscard]] virtual QByteArray getPayload() const
		{
			return QByteArray();
		}


		void run() override;
		void reportCommunicationError(const GlobalStatus& pStatus);
		void checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration);

	private Q_SLOTS:
		void onSslErrors(const QList<QSslError>& pErrors);
		void onSslHandshakeDone();
		void onNetworkReply();

	public:
		void onExit(QEvent* pEvent) override;
};

} // namespace governikus
