/*!
 * \brief A CV certificate chain is an ordered list, where the
 * CV certificates are ordered by the holder to authority reference
 * relation
 * (The holder is equivalent to the Subject-DN, the authority reference
 * is equivalent to the Issuer-DN.)
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "CVCertificate.h"
#include "Commands.h"

#include <QVector>

namespace governikus
{

class CVCertificateChain
	: public QVector<QSharedPointer<CVCertificate> >
{
	private:
		bool mProductive;

	public:
		CVCertificateChain(bool pProductive = true);

		CVCertificateChain(const QVector<QSharedPointer<CVCertificate> >& pCvcs, bool pProductive);

		/*!
		 * Return the document verifier certificate.
		 */
		QSharedPointer<CVCertificate> getDvCvc() const;

		/*!
		 * Return the terminal certificate.
		 */
		QSharedPointer<CVCertificate> getTerminalCvc() const;

		/*!
		 * Is this a valid CVC chain, i.e. do DV and terminal CVCs exist?
		 */
		bool isValid() const;

		/*!
		 * Is this a CVC chain for productive usage?
		 */
		bool isProductive() const;
};

} /* namespace governikus */
