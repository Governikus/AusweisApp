/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief A CV certificate chain is an ordered list, where the
 * CV certificates are ordered by the holder to authority reference
 * relation
 * (The holder is equivalent to the Subject-DN, the authority reference
 * is equivalent to the Issuer-DN.)
 */

#pragma once

#include "CVCertificate.h"

#include <QList>

namespace governikus
{

class CVCertificateChain
	: public QList<QSharedPointer<const CVCertificate>>
{
	private:
		bool mProductive;

	public:
		explicit CVCertificateChain(bool pProductive = true);
		explicit CVCertificateChain(const QList<QSharedPointer<const CVCertificate>>& pCvcs, bool pProductive);

		/*!
		 * Return the document verifier certificate.
		 */
		[[nodiscard]] QSharedPointer<const CVCertificate> getDvCvc() const;

		/*!
		 * Return the terminal certificate.
		 */
		[[nodiscard]] QSharedPointer<const CVCertificate> getTerminalCvc() const;

		/*!
		 * Is this a valid CVC chain, i.e. do DV and terminal CVCs exist?
		 */
		[[nodiscard]] bool isValid() const;

		/*!
		 * Is this a CVC chain for productive usage?
		 */
		[[nodiscard]] bool isProductive() const;
};

} // namespace governikus
