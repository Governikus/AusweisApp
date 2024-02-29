/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of Certificate Holder Authorization Template, CHAT.
 */

#pragma once

#include "ASN1TemplateUtil.h"
#include "AccessRoleAndRight.h"
#include "Oid.h"

#include <openssl/asn1t.h>

#include <QByteArray>
#include <QSharedPointer>

namespace governikus
{

/*!
 * According to
 *    - TR-03110-3, chapter C.1.5 and
 *    - TR-03110-3, chapter D.2 and
 *    - TR-03110-4, chapter 2.2.3.2 and
 * a Certificate Holder Authorization Template for Authentication Terminals
 * has the following structure:
 *
 *
 * Chat ::= APPLICATION [0x4C] IMPLICIT SEQUENCE {
 *      type OBJECT IDENTIFIER,
 *      template DiscretionaryData
 * }
 *
 * DiscretionaryData ::= APPLICATION [0x13] IMPLICIT OCTET STRING SIZE(5)
 *
 * where the DiscretionaryData bits have the following meaning:
 *     39 38 37 ... 32 31 30 29 ... 8 7 6 5 4 3 2 1 0 Description
 *      x  x  -   -  -  -  -  -   - - - - - - - - - - Role
 *      1  1  -   -  -  -  -  -   - - - - - - - - - - CVCA
 *      1  0  -   -  -  -  -  -   - - - - - - - - - - DV (official domestic)
 *      0  1  -   -  -  -  -  -   - - - - - - - - - - DV (non-official / foreign)
 *      0  0  -   -  -  -  -  -   - - - - - - - - - - Authentication Terminal
 *      -  -  x   x  x  -  -  -   - - - - - - - - - - Write Access (eID)
 *      -  -  1   -  -  -  -  -   - - - - - - - - - - DG 17
 *      -  -  - ...  -  -  -  -   - - - - - - - - - - ...
 *      -  -  -   -  1  -  -  -   - - - - - - - - - - DG 22
 *      -  -  -   -  -  x  x  -   - - - - - - - - - - RFU: R/W Access
 *      -  -  -   -  -  -  -  x   x x - - - - - - - - Read Access (eID)
 *      -  -  -   -  -  -  -  1   - - - - - - - - - - DG 22
 *      -  -  -   -  -  -  -  - ... - - - - - - - - - ...
 *      -  -  -   -  -  -  -  -   - 1 - - - - - - - - DG 1
 *      -  -  -   -  -  -  -  -   - - x x x x x x x x Special Functions
 *      -  -  -   -  -  -  -  -   - - 1 - - - - - - - Install Qualified Certificate
 *      -  -  -   -  -  -  -  -   - - - 1 - - - - - - Install Certificate
 *      -  -  -   -  -  -  -  -   - - - - 1 - - - - - PIN Management
 *      -  -  -   -  -  -  -  -   - - - - - 1 - - - - CAN allowed
 *      -  -  -   -  -  -  -  -   - - - - - - 1 - - - Privileged Terminal
 *      -  -  -   -  -  -  -  -   - - - - - - - 1 - - Restricted Identification
 *      -  -  -   -  -  -  -  -   - - - - - - - - 1 - Municipality ID Verification
 *      -  -  -   -  -  -  -  -   - - - - - - - - - 1 Age Verification
 *
 */


using CHAT = struct chat_st
{
	ASN1_OBJECT* mType;
	ASN1_OCTET_STRING* mTemplate;

	static QSharedPointer<chat_st> fromHex(const QByteArray& pHexValue);
	static QSharedPointer<chat_st> decode(const QByteArray& pBytes);

	chat_st(const chat_st& pCopy);
	chat_st& operator=(const chat_st&) = delete;
	~chat_st();

	[[nodiscard]] Oid getType() const;
	[[nodiscard]] QByteArray getTemplate() const;
	[[nodiscard]] AccessRole getAccessRole() const;

	[[nodiscard]] QSet<AccessRight> getAccessRights() const;
	void setAccessRights(const QSet<AccessRight>& pAccessRights);
	[[nodiscard]] bool hasAccessRight(AccessRight pAccessRight) const;
	void removeAllAccessRights();
	void removeAccessRight(AccessRight pAccessRight);

	private:
		void setTemplateBit(uint pBitIndex, bool pOn);

	public:
		static int decodeCallback(int pOperation, ASN1_VALUE** pVal, const ASN1_ITEM* pIt, void* pExarg);
};

DECLARE_ASN1_FUNCTIONS(CHAT)
DECLARE_ASN1_OBJECT(CHAT)

} // namespace governikus
