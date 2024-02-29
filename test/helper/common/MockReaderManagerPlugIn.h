/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Mock implementation of ReaderManagerPlugIn
 */

#pragma once

#include "MockReader.h"
#include "ReaderManagerPlugIn.h"

#include <QMap>
#include <QString>


namespace governikus
{

class MockReaderManagerPlugIn
	: public ReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "MockReaderManagerPlugIn.metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

	private:
		static MockReaderManagerPlugIn* mInstance;
		QMap<QString, MockReader*> mReaders;

	public:
		MockReaderManagerPlugIn();
		~MockReaderManagerPlugIn() override = default;

		static MockReaderManagerPlugIn& getInstance();

		void insert(const QString& pReaderName, const QVariant& pData) override;
		[[nodiscard]] QList<Reader*> getReaders() const override;
		void startScan(bool pAutoConnect) override;
		MockReader* addReader(const QString& pReaderName = QStringLiteral("MockReader"), ReaderManagerPlugInType pType = MockReader::cMOCKED_READERMANAGER_TYPE);
		void removeReader(const QString& pReaderName);
		void removeAllReader();

		using ReaderManagerPlugIn::setPlugInAvailable;
};


} // namespace governikus
