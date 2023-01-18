/*!
 * \brief Self authentication context.
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"

#include "SelfAuthenticationData.h"

namespace governikus
{

class SelfAuthContext
	: public AuthContext
{
	Q_OBJECT

	private:
		SelfAuthenticationData mSelfAuthenticationData;

	Q_SIGNALS:
		void fireSelfAuthenticationDataChanged();

	public:
		SelfAuthContext();

		[[nodiscard]] const SelfAuthenticationData& getSelfAuthenticationData() const
		{
			return mSelfAuthenticationData;
		}


		void setSelfAuthenticationData(const SelfAuthenticationData& pSelfAuthenticationData)
		{
			mSelfAuthenticationData = pSelfAuthenticationData;
			Q_EMIT fireSelfAuthenticationDataChanged();
		}


};

} // namespace governikus
