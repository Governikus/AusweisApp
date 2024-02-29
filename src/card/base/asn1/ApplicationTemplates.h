/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of the ISO 7816 EF.DIR data structure (set of
 *        application template)
 */

#pragma once

#include "ApplicationTemplate.h"

#include <QByteArray>
#include <QList>
#include <QSharedPointer>


namespace governikus
{

/*!
 * EF.DIR = ApplicationTemplates ::= SET OF ApplicationTemplate

 * defined in ISO 7816-4:2005 8.2.1.1
 */

using ApplicationTemplatesInternal = STACK_OF(ApplicationTemplate);
DECLARE_ASN1_FUNCTIONS(ApplicationTemplatesInternal)
DECLARE_ASN1_OBJECT(ApplicationTemplatesInternal)

class ApplicationTemplates
{
	Q_DISABLE_COPY(ApplicationTemplates)
	friend class QSharedPointer<ApplicationTemplates>;

	private:
		const QByteArray mContentBytes;
		const QList<QSharedPointer<const ApplicationTemplate>> mApplicationTemplates;

		ApplicationTemplates(const QByteArray& pBytes,
				const QList<QSharedPointer<const ApplicationTemplate>>& pApplicationTemplates);

		static QByteArray buildAsn1LengthField(unsigned int pLength);

	public:
		static QSharedPointer<ApplicationTemplates> decode(const QByteArray& pBytes);

		bool contains(const QByteArray& pIdentifier);

		[[nodiscard]] const QByteArray& getContentBytes() const;
		[[nodiscard]] const QList<QSharedPointer<const ApplicationTemplate>>& getApplicationTemplates() const;
		[[nodiscard]] qsizetype count() const;
};


} // namespace governikus
