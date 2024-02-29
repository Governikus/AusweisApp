/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Helper to use AuthContext in an easy way.
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
		QList<AcceptedEidType> mAcceptedEidTypes;

		const QSharedPointer<const CVCertificate> getTerminalCvc(QSharedPointer<DIDAuthenticateEAC1> pEac1) const;

	public:
		explicit TestAuthContext(const QString& pFileName = QString());
		~TestAuthContext() override;

		void setRequiredAccessRights(const QSet<AccessRight>& pAccessRights);
		void setOptionalAccessRights(const QSet<AccessRight>& pAccessRights);
		void addCvCertificate(const QSharedPointer<const CVCertificate>& pCvCertificate);
		void clearCvCertificates();
		void removeCvCertAt(int pPosition);
		[[nodiscard]] virtual QList<AcceptedEidType> getAcceptedEidTypes() const override;
		void setAcceptedEidTypes(const QList<AcceptedEidType>& pAcceptedEidTypes);
};

} // namespace governikus
