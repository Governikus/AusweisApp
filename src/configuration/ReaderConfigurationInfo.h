/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UpdatableFile.h"

#include <QCoreApplication>
#include <QSharedData>
#include <QString>


namespace governikus
{
class ReaderConfigurationInfo
{
	Q_DECLARE_TR_FUNCTIONS(ReaderConfigurationInfo)

	private:
		class InternalInfo
			: public QSharedData
		{
			public:
				const bool mKnown;
				const uint mVendorId;
				const QSet<uint> mProductIds;
				const QString mName;
				const QString mUrl;
				const QString mPattern;
				const QString mIcon;


				InternalInfo(bool pKnown, uint pVendorId, const QSet<uint>& pProductIds, const QString& pName, const QString& pUrl,
						const QString& pPattern, const QString& pIcon)
					: mKnown(pKnown)
					, mVendorId(pVendorId)
					, mProductIds(pProductIds)
					, mName(pName)
					, mUrl(pUrl)
					, mPattern(pPattern)
					, mIcon(pIcon)
				{
				}


				bool operator ==(const InternalInfo& pOther) const
				{
					return !(mKnown != pOther.mKnown ||
						   mVendorId != pOther.mVendorId ||
						   mProductIds != pOther.mProductIds ||
						   mName != pOther.mName ||
						   mUrl != pOther.mUrl ||
						   mPattern != pOther.mPattern ||
						   mIcon != pOther.mIcon);
				}


		};

		QSharedDataPointer<InternalInfo> d;

	public:
		ReaderConfigurationInfo();
		explicit ReaderConfigurationInfo(const QString& pReaderName);
		ReaderConfigurationInfo(uint pVendorId, const QSet<uint>& pProductIds,
				const QString& pName, const QString& pUrl, const QString& pPattern,
				const QString& pIcon);

		virtual ~ReaderConfigurationInfo();

		bool operator ==(const ReaderConfigurationInfo& pOther) const;

		[[nodiscard]] bool isKnownReader() const;
		[[nodiscard]] uint getVendorId() const;
		[[nodiscard]] QSet<uint> getProductIds() const;
		[[nodiscard]] const QString& getName() const;
		[[nodiscard]] const QString& getUrl() const;
		[[nodiscard]] const QString& getPattern() const;
		[[nodiscard]] QSharedPointer<UpdatableFile> getIcon() const;
};


inline auto qHash(const ReaderConfigurationInfo& info)
{
	return qHash(info.getName());
}


} // namespace governikus
