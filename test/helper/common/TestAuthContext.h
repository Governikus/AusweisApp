/*!
 * \brief Helper to use AuthContext in an easy way.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
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

	public:
		explicit TestAuthContext(const QSharedPointer<ActivationContext>& pActivationContext, const QString& pFileName = QString());
		~TestAuthContext() override;

		void setRequiredAccessRights(const QSet<AccessRight>& pAccessRights);
		void setOptionalAccessRights(const QSet<AccessRight>& pAccessRights);
		void addCvCertificate(const QSharedPointer<const CVCertificate>& pCvCertificate);
		void clearCvCertificates();
		void removeCvCertAt(int pPosition);
};

} // namespace governikus
