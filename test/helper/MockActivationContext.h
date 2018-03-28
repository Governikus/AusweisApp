/*!
 * \brief Mocked ActivationContext for unit tests.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "ActivationContext.h"
#include "ActivationHandler.h"


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
		virtual ~MockActivationContext() override;


		virtual QUrl getActivationURL() const override
		{
			return QUrl();
		}


		virtual bool sendProcessing() override
		{
			mSendProcessingCalled = true;
			mSendError = mErrorMessageOnSend;
			return mProcessingValue;
		}


		virtual bool sendOperationAlreadyActive() override
		{
			mSendAlreadyActiveCalled = true;
			mSendError = mErrorMessageOnSend;
			return mAlreadyActiveValue;
		}


		virtual bool sendErrorPage(HttpStatusCode pStatusCode, const GlobalStatus& pStatus) override
		{
			Q_UNUSED(pStatusCode);
			Q_UNUSED(pStatus);
			mSendErroPageCalled = true;
			mSendError = mErrorMessageOnSend;
			return mErroPageValue;
		}


		virtual bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus) override
		{
			Q_UNUSED(pRedirectAddress);
			Q_UNUSED(pStatus);
			mSendRedirectCalled = true;
			mSendError = mErrorMessageOnSend;
			return mRedirectValue;
		}


		bool isSendAlreadyActiveCalled() const
		{
			return mSendAlreadyActiveCalled;
		}


		bool isSendErroPageCalled() const
		{
			return mSendErroPageCalled;
		}


		bool isSendProcessingCalled() const
		{
			return mSendProcessingCalled;
		}


		bool isSendRedirectCalled() const
		{
			return mSendRedirectCalled;
		}


};


} /* namespace governikus */
