/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QtGlobal>


namespace governikus
{

class UsbId
{
	private:
		unsigned int mVendorId;
		unsigned int mProductId;

	public:
		UsbId(unsigned int pVendorId = 0x0, unsigned int pProductId = 0x0);

		[[nodiscard]] unsigned int getVendorId() const;
		[[nodiscard]] unsigned int getProductId() const;

		bool operator==(const UsbId& pOther) const;
};

} // namespace governikus

Q_DECLARE_TYPEINFO(governikus::UsbId, Q_PRIMITIVE_TYPE);

QDebug operator <<(QDebug pDbg, const governikus::UsbId& pUsbId);
