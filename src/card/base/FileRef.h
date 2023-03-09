/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Reference information for files on smart cards.
 */

#pragma once

#include <QByteArray>
#include <QDebug>


namespace governikus
{

class FileRef
{
	public:
		enum TYPE : uchar
		{
			MASTER_FILE = 0x00,
			ELEMENTARY_FILE = 0x02,
			APPLICATION = 0x04,
			UNKNOWN = 0xFF
		};

	private:
		TYPE mType;
		QByteArray mIdentifier;
		QByteArray mShortIdentifier;

	public:
		static FileRef masterFile();
		static FileRef efDir();
		static FileRef efCardAccess();
		static FileRef efCardSecurity();
		static FileRef appCIA();
		static FileRef appEId();
		static FileRef appESign();
		static FileRef appPassport();
		static FileRef appPersosim();

		FileRef();
		FileRef(uchar pType, const QByteArray& pIdentifier, const QByteArray& pShortIdentifier = QByteArray());
		FileRef(TYPE pType, const QByteArray& pIdentifier, const QByteArray& pShortIdentifier = QByteArray());

		[[nodiscard]] TYPE getType() const;
		[[nodiscard]] const QByteArray& getIdentifier() const;
		[[nodiscard]] const QByteArray& getShortIdentifier() const;
		[[nodiscard]] QString getName() const;
};


inline QDebug operator<<(QDebug pDbg, const FileRef& pFileRef)
{
	QDebugStateSaver saver(pDbg);
	pDbg.noquote().nospace() << pFileRef.getIdentifier().toHex().toUpper() << " (" << pFileRef.getName() << ")";
	return pDbg;
}


} // namespace governikus
