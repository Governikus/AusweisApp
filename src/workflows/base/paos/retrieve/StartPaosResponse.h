/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "paos/ResponseType.h"
#include "paos/element/ElementParser.h"

#include <QByteArray>
#include <QString>

namespace governikus
{

class StartPaosResponse
	: public ResponseType
{


	private:
		const QSharedPointer<ElementParser> mParser;
		QString mResultMajor;
		QString mResultMinor;
		QString mResultMessage;

		void parse();
		void parseResult();

	public:
		explicit StartPaosResponse(const QSharedPointer<ElementParser>& pParser);
		~StartPaosResponse() override;
};

} // namespace governikus
