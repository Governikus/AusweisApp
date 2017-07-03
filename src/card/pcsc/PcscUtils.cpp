/*!
 * PcscUtils.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "PcscUtils.h"

using namespace governikus;


QString PcscUtils::toString(PCSC_RETURNCODE pCode)
{
	switch (pCode)
	{
		case Scard_S_Success:
			return QStringLiteral("SCARD_S_SUCCESS");

		case Scard_F_Internal_Error:
			return QStringLiteral("SCARD_F_INTERNAL_ERROR");

		case Scard_E_Cancelled:
			return QStringLiteral("SCARD_E_CANCELLED");

		case Scard_E_Invalid_Handle:
			return QStringLiteral("SCARD_E_INVALID_HANDLE");

		case Scard_E_Invalid_Parameter:
			return QStringLiteral("SCARD_E_INVALID_PARAMETER");

		case Scard_E_Invalid_Target:
			return QStringLiteral("SCARD_E_INVALID_TARGET");

		case Scard_E_No_Memory:
			return QStringLiteral("SCARD_E_NO_MEMORY");

		case Scard_F_Waited_Too_Long:
			return QStringLiteral("SCARD_F_WAITED_TOO_LONG");

		case Scard_E_Insufficient_Buffer:
			return QStringLiteral("SCARD_E_INSUFFICIENT_BUFFER");

		case Scard_E_Unknown_Reader:
			return QStringLiteral("SCARD_E_UNKNOWN_READER");

		case Scard_E_Timeout:
			return QStringLiteral("SCARD_E_TIMEOUT");

		case Scard_E_Sharing_Violation:
			return QStringLiteral("SCARD_E_SHARING_VIOLATION");

		case Scard_E_No_Smartcard:
			return QStringLiteral("SCARD_E_NO_SMARTCARD");

		case Scard_E_Unknown_Card:
			return QStringLiteral("SCARD_E_UNKNOWN_CARD");

		case Scard_E_Cant_Dispose:
			return QStringLiteral("SCARD_E_CANT_DISPOSE");

		case Scard_E_Proto_Mismatch:
			return QStringLiteral("SCARD_E_PROTO_MISMATCH");

		case Scard_E_Not_Ready:
			return QStringLiteral("SCARD_E_NOT_READY");

		case Scard_E_Invalid_Value:
			return QStringLiteral("SCARD_E_INVALID_VALUE");

		case Scard_E_System_Cancelled:
			return QStringLiteral("SCARD_E_SYSTEM_CANCELLED");

		case Scard_F_Comm_Error:
			return QStringLiteral("SCARD_F_COMM_ERROR");

		case Scard_F_Unknown_Error:
			return QStringLiteral("SCARD_F_UNKNOWN_ERROR");

		case Scard_E_Invalid_Atr:
			return QStringLiteral("SCARD_E_INVALID_ATR");

		case Scard_E_Not_Transacted:
			return QStringLiteral("SCARD_E_NOT_TRANSACTED");

		case Scard_E_Reader_Unavailable:
			return QStringLiteral("SCARD_E_READER_UNAVAILABLE");

		case Scard_P_Shutdown:
			return QStringLiteral("SCARD_P_SHUTDOWN");

		case Scard_E_Pci_Too_Small:
			return QStringLiteral("SCARD_E_PCI_TOO_SMALL");

		case Scard_E_Reader_Unsupported:
			return QStringLiteral("SCARD_E_READER_UNSUPPORTED");

		case Scard_E_Duplicate_Reader:
			return QStringLiteral("SCARD_E_DUPLICATE_READER");

		case Scard_E_Card_Unsupported:
			return QStringLiteral("SCARD_E_CARD_UNSUPPORTED");

		case Scard_E_No_Service:
			return QStringLiteral("SCARD_E_NO_SERVICE");

		case Scard_E_Service_Stopped:
			return QStringLiteral("SCARD_E_SERVICE_STOPPED");

		case Scard_E_Icc_Installation:
			return QStringLiteral("SCARD_E_ICC_INSTALLATION");

		case Scard_E_Icc_Createorder:
			return QStringLiteral("SCARD_E_ICC_CREATEORDER");

		// Scard_E_Unexpected has the same value as Scard_E_Unsupported_Feature, no
		// separate case for it.
		case Scard_E_Unsupported_Feature:
			return QStringLiteral("SCARD_E_UNSUPPORTED_FEATURE");

		case Scard_E_Dir_Not_Found:
			return QStringLiteral("SCARD_E_DIR_NOT_FOUND");

		case Scard_E_File_Not_Found:
			return QStringLiteral("SCARD_E_FILE_NOT_FOUND");

		case Scard_E_No_Dir:
			return QStringLiteral("SCARD_E_NO_DIR");

		case Scard_E_No_File:
			return QStringLiteral("SCARD_E_NO_FILE");

		case Scard_E_No_Access:
			return QStringLiteral("SCARD_E_NO_ACCESS");

		case Scard_E_Write_Too_Many:
			return QStringLiteral("SCARD_E_WRITE_TOO_MANY");

		case Scard_E_Bad_Seek:
			return QStringLiteral("SCARD_E_BAD_SEEK");

		case Scard_E_Invalid_Chv:
			return QStringLiteral("SCARD_E_INVALID_CHV");

		case Scard_E_Unknown_Res_Mng:
			return QStringLiteral("SCARD_E_UNKNOWN_RES_MNG");

		case Scard_E_No_Such_Certificate:
			return QStringLiteral("SCARD_E_NO_SUCH_CERTIFICATE");

		case Scard_E_Certificate_Unavailable:
			return QStringLiteral("SCARD_E_CERTIFICATE_UNAVAILABLE");

		case Scard_E_No_Readers_Available:
			return QStringLiteral("SCARD_E_NO_READERS_AVAILABLE");

		case Scard_E_Comm_Data_Lost:
			return QStringLiteral("SCARD_E_COMM_DATA_LOST");

		case Scard_E_No_Key_Container:
			return QStringLiteral("SCARD_E_NO_KEY_CONTAINER");

		case Scard_E_Server_Too_Busy:
			return QStringLiteral("SCARD_E_SERVER_TOO_BUSY");

		case Scard_W_Unsupported_Card:
			return QStringLiteral("SCARD_W_UNSUPPORTED_CARD");

		case Scard_W_Unresponsive_Card:
			return QStringLiteral("SCARD_W_UNRESPONSIVE_CARD");

		case Scard_W_Unpowered_Card:
			return QStringLiteral("SCARD_W_UNPOWERED_CARD");

		case Scard_W_Reset_Card:
			return QStringLiteral("SCARD_W_RESET_CARD");

		case Scard_W_Removed_Card:
			return QStringLiteral("SCARD_W_REMOVED_CARD");

		case Scard_W_Security_Violation:
			return QStringLiteral("SCARD_W_SECURITY_VIOLATION");

		case Scard_W_Wrong_Chv:
			return QStringLiteral("SCARD_W_WRONG_CHV");

		case Scard_W_Chv_Blocked:
			return QStringLiteral("SCARD_W_CHV_BLOCKED");

		case Scard_W_Eof:
			return QStringLiteral("SCARD_W_EOF");

		case Scard_W_Cancelled_By_User:
			return QStringLiteral("SCARD_W_CANCELLED_BY_USER");

		case Scard_W_Card_Not_Authenticated:
			return QStringLiteral("SCARD_W_CARD_NOT_AUTHENTICATED");

		default:
			return QStringLiteral("UNKNOWN_STATE (%1)").arg(pCode, 8, 16, QChar('0'));
	}
}
