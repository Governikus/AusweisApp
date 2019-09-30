/*!
 * \brief Helper to use AuthContext in an easy way.
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
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
		QSharedPointer<DIDAuthenticateEAC2> mDidAuthenticateEac2;

	public:
		TestAuthContext(ActivationContext* pActivationContext, const QString& pFileName);
		virtual ~TestAuthContext();

		void setRequiredAccessRights(const QSet<AccessRight>& pAccessRights);
		void setOptionalAccessRights(const QSet<AccessRight>& pAccessRights);
		void addCvCertificate(const QSharedPointer<const CVCertificate>& pCvCertificate);
		void clearCvCertificates();
		void removeCvCertAt(int pPosition);
};

} // namespace governikus
