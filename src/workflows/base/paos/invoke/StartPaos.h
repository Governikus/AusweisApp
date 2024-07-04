/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Generate information for StartPaos.
 */

#pragma once

#include "paos/PaosMessage.h"
#include "paos/element/ConnectionHandle.h"
#include "paos/invoke/PaosCreator.h"

class test_StartPaos;

namespace governikus
{

class StartPaos
	: public PaosCreator
	, public PaosMessage
{
	Q_DISABLE_COPY(StartPaos)
	friend class ::test_StartPaos;

	private:
		const QByteArray mSessionId;

		void createBodyElement(QXmlStreamWriter& pWriter) override;
		void createSessionIdentifierElement();
		void createConnectionHandleElement(QXmlStreamWriter& pWriter) const;
		void createUserAgentElement(QXmlStreamWriter& pWriter) const;
		void createSupportedAPIVersionsElement(QXmlStreamWriter& pWriter) const;

	public:
		explicit StartPaos(const QByteArray& pSessionId);
};

} // namespace governikus
