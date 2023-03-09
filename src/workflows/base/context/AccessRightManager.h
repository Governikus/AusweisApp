/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Manager for AccessRights
 */

#pragma once

#include "asn1/CVCertificate.h"
#include "paos/retrieve/DidAuthenticateEac1.h"

#include <QDebug>
#include <QObject>
#include <QPointer>
#include <QSharedPointer>

class test_ChatModel;

namespace governikus
{

class AccessRightManager
	: public QObject
{
	Q_OBJECT
	friend class ::test_ChatModel;

	public:
		explicit AccessRightManager(QSharedPointer<DIDAuthenticateEAC1> pDIDAuthenticateEAC1, QSharedPointer<const CVCertificate> pTerminalCvc);


		[[nodiscard]] const QSharedPointer<const CVCertificate>& getTerminalCvc() const
		{
			return mTerminalCvc;
		}


		[[nodiscard]] const QSet<AccessRight>& getOptionalAccessRights() const
		{
			return mOptionalAccessRights;
		}


		[[nodiscard]] const QSet<AccessRight>& getRequiredAccessRights() const
		{
			return mRequiredAccessRights;
		}


		[[nodiscard]] const QSet<AccessRight>& getEffectiveAccessRights() const
		{
			return mEffectiveAccessRights;
		}


		void operator+=(AccessRight pAccessRight);
		void operator-=(AccessRight pAccessRight);
		void operator=(const QSet<AccessRight>& pAccessRights);
		operator QByteArray() const;

	Q_SIGNALS:
		void fireEffectiveAccessRightsChanged();

	private:
		QSharedPointer<const CVCertificate> mTerminalCvc;
		QSharedPointer<DIDAuthenticateEAC1> mDIDAuthenticateEAC1;
		QSet<AccessRight> mOptionalAccessRights;
		QSet<AccessRight> mEffectiveAccessRights;
		QSet<AccessRight> mRequiredAccessRights;

		void removeForbiddenAccessRights(QSet<AccessRight>& pAccessRights);
};

} // namespace governikus
