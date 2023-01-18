/*!
 * \brief Mocked ActivationContext for unit tests.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/ActivationContext.h"


namespace governikus
{

class MockActivationContext
	: public ActivationContext
{
	Q_OBJECT

	bool mProcessingValue, mAlreadyActiveValue, mErroPageValue, mRedirectValue;
	const QString mErrorMessageOnSend;
	bool mSendProcessingCalled, mSendAlreadyActiveCalled, mSendErroPageCalled, mSendRedirectCalled;

	public:
		MockActivationContext(bool pProcessing = false, bool pAlreadyActive = false, bool pErroPage = false, bool pRedirect = false, const QString& pSendError = QString());
		~MockActivationContext() override;


		[[nodiscard]] QUrl getActivationURL() const override
		{
			return QUrl();
		}


		bool sendProcessing() override
		{
			mSendProcessingCalled = true;
			setSendError(mErrorMessageOnSend);
			return mProcessingValue;
		}


		bool sendOperationAlreadyActive() override
		{
			mSendAlreadyActiveCalled = true;
			setSendError(mErrorMessageOnSend);
			return mAlreadyActiveValue;
		}


		bool sendErrorPage(http_status pStatusCode, const GlobalStatus& pStatus) override
		{
			Q_UNUSED(pStatusCode)
			Q_UNUSED(pStatus)
			mSendErroPageCalled = true;
			setSendError(mErrorMessageOnSend);
			return mErroPageValue;
		}


		bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus) override
		{
			Q_UNUSED(pRedirectAddress)
			Q_UNUSED(pStatus)
			mSendRedirectCalled = true;
			setSendError(mErrorMessageOnSend);
			return mRedirectValue;
		}


		[[nodiscard]] bool isSendAlreadyActiveCalled() const
		{
			return mSendAlreadyActiveCalled;
		}


		[[nodiscard]] bool isSendErroPageCalled() const
		{
			return mSendErroPageCalled;
		}


		[[nodiscard]] bool isSendProcessingCalled() const
		{
			return mSendProcessingCalled;
		}


		[[nodiscard]] bool isSendRedirectCalled() const
		{
			return mSendRedirectCalled;
		}


};


} // namespace governikus
