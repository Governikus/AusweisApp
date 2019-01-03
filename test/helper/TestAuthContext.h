/*!
 * \brief Helper to use AuthContext in an easy way.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"

#include <QString>

namespace governikus
{

class TestAuthContext
	: public AuthContext
{
	Q_OBJECT

	private:
		QSharedPointer<DIDAuthenticateEAC1> mDidAuthenticateEac1;

	public:
		TestAuthContext(ActivationContext* pActivationContext, const QString& pFileName);
		virtual ~TestAuthContext();

		void setRequiredAccessRights(const QSet<AccessRight>& pAccessRights);
		void setOptionalAccessRights(const QSet<AccessRight>& pAccessRights);
};

} // namespace governikus
