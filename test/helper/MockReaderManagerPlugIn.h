/*!
 * MockReaderManagerPlugIn.h
 *
 * \brief Mock implementation of ReaderManagerPlugIn
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */


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

	public:
		static MockReaderManagerPlugIn* mInstance;
		QMap<QString, MockReader*> mReaders;

		MockReaderManagerPlugIn();

		static MockReaderManagerPlugIn& getInstance();

		QList<Reader*> getReader() const;

		MockReader* addReader(const QString& pReaderName);

		void removeReader(const QString& pReaderName);


};


}
