/*!
 * PcscUtils.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "PcscUtils.h"

using namespace governikus;


QString PcscUtils::toString(PCSC_RETURNCODE pCode)
{
	switch (static_cast<uint>(pCode))
	{
		case SCARD_S_SUCCESS:
			return QStringLiteral("SCARD_S_SUCCESS");

		case SCARD_F_INTERNAL_ERROR:
			return QStringLiteral("SCARD_F_INTERNAL_ERROR");

		case SCARD_E_CANCELLED:
			return QStringLiteral("SCARD_E_CANCELLED");

		case SCARD_E_INVALID_HANDLE:
			return QStringLiteral("SCARD_E_INVALID_HANDLE");

		case SCARD_E_INVALID_PARAMETER:
			return QStringLiteral("SCARD_E_INVALID_PARAMETER");

		case SCARD_E_INVALID_TARGET:
			return QStringLiteral("SCARD_E_INVALID_TARGET");

		case SCARD_E_NO_MEMORY:
			return QStringLiteral("SCARD_E_NO_MEMORY");

		case SCARD_F_WAITED_TOO_LONG:
			return QStringLiteral("SCARD_F_WAITED_TOO_LONG");

		case SCARD_E_INSUFFICIENT_BUFFER:
			return QStringLiteral("SCARD_E_INSUFFICIENT_BUFFER");

		case SCARD_E_UNKNOWN_READER:
			return QStringLiteral("SCARD_E_UNKNOWN_READER");

		case SCARD_E_TIMEOUT:
			return QStringLiteral("SCARD_E_TIMEOUT");

		case SCARD_E_SHARING_VIOLATION:
			return QStringLiteral("SCARD_E_SHARING_VIOLATION");

		case SCARD_E_NO_SMARTCARD:
			return QStringLiteral("SCARD_E_NO_SMARTCARD");

		case SCARD_E_UNKNOWN_CARD:
			return QStringLiteral("SCARD_E_UNKNOWN_CARD");

		case SCARD_E_CANT_DISPOSE:
			return QStringLiteral("SCARD_E_CANT_DISPOSE");

		case SCARD_E_PROTO_MISMATCH:
			return QStringLiteral("SCARD_E_PROTO_MISMATCH");

		case SCARD_E_NOT_READY:
			return QStringLiteral("SCARD_E_NOT_READY");

		case SCARD_E_INVALID_VALUE:
			return QStringLiteral("SCARD_E_INVALID_VALUE");

		case SCARD_E_SYSTEM_CANCELLED:
			return QStringLiteral("SCARD_E_SYSTEM_CANCELLED");

		case SCARD_F_COMM_ERROR:
			return QStringLiteral("SCARD_F_COMM_ERROR");

		case SCARD_F_UNKNOWN_ERROR:
			return QStringLiteral("SCARD_F_UNKNOWN_ERROR");

		case SCARD_E_INVALID_ATR:
			return QStringLiteral("SCARD_E_INVALID_ATR");

		case SCARD_E_NOT_TRANSACTED:
			return QStringLiteral("SCARD_E_NOT_TRANSACTED");

		case SCARD_E_READER_UNAVAILABLE:
			return QStringLiteral("SCARD_E_READER_UNAVAILABLE");

		case SCARD_P_SHUTDOWN:
			return QStringLiteral("SCARD_P_SHUTDOWN");

		case SCARD_E_PCI_TOO_SMALL:
			return QStringLiteral("SCARD_E_PCI_TOO_SMALL");

		case SCARD_E_READER_UNSUPPORTED:
			return QStringLiteral("SCARD_E_READER_UNSUPPORTED");

		case SCARD_E_DUPLICATE_READER:
			return QStringLiteral("SCARD_E_DUPLICATE_READER");

		case SCARD_E_CARD_UNSUPPORTED:
			return QStringLiteral("SCARD_E_CARD_UNSUPPORTED");

		case SCARD_E_NO_SERVICE:
			return QStringLiteral("SCARD_E_NO_SERVICE");

		case SCARD_E_SERVICE_STOPPED:
			return QStringLiteral("SCARD_E_SERVICE_STOPPED");

		case SCARD_E_UNEXPECTED:
			return QStringLiteral("SCARD_E_UNEXPECTED");

		case SCARD_E_ICC_INSTALLATION:
			return QStringLiteral("SCARD_E_ICC_INSTALLATION");

		case SCARD_E_ICC_CREATEORDER:
			return QStringLiteral("SCARD_E_ICC_CREATEORDER");

		//case SCARD_E_UNSUPPORTED_FEATURE: return QStringLiteral("SCARD_E_UNSUPPORTED_FEATURE");
		case SCARD_E_DIR_NOT_FOUND:
			return QStringLiteral("SCARD_E_DIR_NOT_FOUND");

		case SCARD_E_FILE_NOT_FOUND:
			return QStringLiteral("SCARD_E_FILE_NOT_FOUND");

		case SCARD_E_NO_DIR:
			return QStringLiteral("SCARD_E_NO_DIR");

		case SCARD_E_NO_FILE:
			return QStringLiteral("SCARD_E_NO_FILE");

		case SCARD_E_NO_ACCESS:
			return QStringLiteral("SCARD_E_NO_ACCESS");

		case SCARD_E_WRITE_TOO_MANY:
			return QStringLiteral("SCARD_E_WRITE_TOO_MANY");

		case SCARD_E_BAD_SEEK:
			return QStringLiteral("SCARD_E_BAD_SEEK");

		case SCARD_E_INVALID_CHV:
			return QStringLiteral("SCARD_E_INVALID_CHV");

		case SCARD_E_UNKNOWN_RES_MNG:
			return QStringLiteral("SCARD_E_UNKNOWN_RES_MNG");

		case SCARD_E_NO_SUCH_CERTIFICATE:
			return QStringLiteral("SCARD_E_NO_SUCH_CERTIFICATE");

		case SCARD_E_CERTIFICATE_UNAVAILABLE:
			return QStringLiteral("SCARD_E_CERTIFICATE_UNAVAILABLE");

		case SCARD_E_NO_READERS_AVAILABLE:
			return QStringLiteral("SCARD_E_NO_READERS_AVAILABLE");

		case SCARD_E_COMM_DATA_LOST:
			return QStringLiteral("SCARD_E_COMM_DATA_LOST");

		case SCARD_E_NO_KEY_CONTAINER:
			return QStringLiteral("SCARD_E_NO_KEY_CONTAINER");

		case SCARD_E_SERVER_TOO_BUSY:
			return QStringLiteral("SCARD_E_SERVER_TOO_BUSY");

		case SCARD_W_UNSUPPORTED_CARD:
			return QStringLiteral("SCARD_W_UNSUPPORTED_CARD");

		case SCARD_W_UNRESPONSIVE_CARD:
			return QStringLiteral("SCARD_W_UNRESPONSIVE_CARD");

		case SCARD_W_UNPOWERED_CARD:
			return QStringLiteral("SCARD_W_UNPOWERED_CARD");

		case SCARD_W_RESET_CARD:
			return QStringLiteral("SCARD_W_RESET_CARD");

		case SCARD_W_REMOVED_CARD:
			return QStringLiteral("SCARD_W_REMOVED_CARD");

		case SCARD_W_SECURITY_VIOLATION:
			return QStringLiteral("SCARD_W_SECURITY_VIOLATION");

		case SCARD_W_WRONG_CHV:
			return QStringLiteral("SCARD_W_WRONG_CHV");

		case SCARD_W_CHV_BLOCKED:
			return QStringLiteral("SCARD_W_CHV_BLOCKED");

		case SCARD_W_EOF:
			return QStringLiteral("SCARD_W_EOF");

		case SCARD_W_CANCELLED_BY_USER:
			return QStringLiteral("SCARD_W_CANCELLED_BY_USER");

		case SCARD_W_CARD_NOT_AUTHENTICATED:
			return QStringLiteral("SCARD_W_CARD_NOT_AUTHENTICATED");

		//case SCARD_W_CACHE_ITEM_NOT_FOUND: return QStringLiteral("SCARD_W_CACHE_ITEM_NOT_FOUND");
		//case SCARD_W_CACHE_ITEM_STALE: return QStringLiteral("SCARD_W_CACHE_ITEM_STALE");
		default:
			return QStringLiteral("UNKNOWN_STATE (%1)").arg(pCode, 8, 16, QChar('0'));
	}
}
