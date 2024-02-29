/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Self authentication context.
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
		explicit SelfAuthContext(bool pActivateUi = true);

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
