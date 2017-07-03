/*!
 * SelfAuthenticationContext.h
 *
 * \brief Self authentication context.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "AuthContext.h"


namespace governikus
{

class SelfAuthenticationContext
	: public AuthContext
{
	Q_OBJECT

	private:
		QSharedPointer<SelfAuthenticationData> mSelfAuthenticationData;

	Q_SIGNALS:
		void fireSelfAuthenticationDataChanged();

	public:
		SelfAuthenticationContext(ActivationContext* pActivationContext);
		virtual ~SelfAuthenticationContext();


		const QSharedPointer<SelfAuthenticationData>& getSelfAuthenticationData() const
		{
			return mSelfAuthenticationData;
		}


		void setSelfAuthenticationData(const QSharedPointer<SelfAuthenticationData>& pSelfAuthenticationData)
		{
			mSelfAuthenticationData = pSelfAuthenticationData;
			Q_EMIT fireSelfAuthenticationDataChanged();
		}


};

} /* namespace governikus */
