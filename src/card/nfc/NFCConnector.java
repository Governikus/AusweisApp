package com.governikus.ausweisapp2;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;


import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.nfc.NfcAdapter;
import android.nfc.tech.IsoDep;
import android.nfc.Tag;
import android.nfc.TagLostException;
import android.util.Log;

public class NFCConnector
{

	public static enum NfcCardCode
	{
		NO_CARD(0),
		SAME_CARD(1),
		NEW_CARD(2);

		private byte mVal;
		NfcCardCode(int pVal)
		{
			mVal = (byte) pVal;
		}
		public byte toByte()
		{
			return mVal;
		}


	}


	public static enum NfcConnectorCode
	{
		SUCCESS(0),
		ERROR_NO_NFC(-1),
		ERROR_NO_TAG(-2),
		ERROR_CONNECT(-3),
		ERROR_TAG_LOST(-4),
		ERROR_TRANSMIT(-5),
		ERROR_UNKNOWN(-16);


		private byte mVal;
		NfcConnectorCode(int pVal)
		{
			mVal = (byte) pVal;
		}
		public byte toByte()
		{
			return mVal;
		}


	}

	private static final byte[] SUCCESS_BYTE = {
		NfcConnectorCode.SUCCESS.toByte()
	};
	private static final byte[] ERROR_NO_NFC_BYTE = {
		NfcConnectorCode.ERROR_NO_NFC.toByte()
	};
	private static final byte[] ERROR_NO_TAG_BYTE = {
		NfcConnectorCode.ERROR_NO_TAG.toByte()
	};
	private static final byte[] ERROR_CONNECT_BYTE = {
		NfcConnectorCode.ERROR_CONNECT.toByte()
	};
	private static final byte[] ERROR_TAG_LOST_BYTE = {
		NfcConnectorCode.ERROR_TAG_LOST.toByte()
	};
	private static final byte[] ERROR_TRANSMIT_BYTE = {
		NfcConnectorCode.ERROR_TRANSMIT.toByte()
	};
	private static final byte[] ERROR_UNKNOWN_BYTE = {
		NfcConnectorCode.ERROR_UNKNOWN.toByte()
	};


	public static enum ExtendedLengthApduSupportCode
	{
		UNKNOWN(-1),
		NOT_SUPPORTED(0),
		SUPPORTED(1);


		private byte mVal;
		ExtendedLengthApduSupportCode(int pVal)
		{
			mVal = (byte) pVal;
		}
		public byte toByte()
		{
			return mVal;
		}


	}

	private static final String LOG_TAG = "AusweisApp2";


	public synchronized void updateNfcTag(Tag inputTag)
	{
		if (inputTag != null&& Arrays.asList(inputTag.getTechList()).contains(IsoDep.class.getName()))
		{
			mTag = IsoDep.get(inputTag);
			newTag = true;
		}
	}


	private Context context = null;

	private NfcAdapter nfcAdapter = null;

	private boolean newTag = false;

	private IsoDep mTag = null;

	private static NFCConnector singleInstance = null;

	private NFCConnector(Context context)
	{
		super();
		this.context = context;
		this.nfcAdapter = NfcAdapter.getDefaultAdapter(this.context);
		start();
	}


	public static synchronized NFCConnector getInstance(Context context)
	{
		if (singleInstance == null)
		{
			Log.d(LOG_TAG, "NfcConnector singleton intialized.");
			singleInstance = new NFCConnector(context);
		}

		return singleInstance;
	}


	public synchronized byte start()
	{
		if (this.nfcAdapter == null)
		{
			return NfcConnectorCode.ERROR_NO_NFC.toByte();
		}

		return NfcConnectorCode.SUCCESS.toByte();
	}


	public synchronized byte stop()
	{
		if (this.mTag != null)
		{
			if (this.mTag.isConnected())
			{
				try
				{
					this.mTag.close();
				}
				catch (IOException e)
				{
					// nothing
				}
			}
			this.mTag = null;
		}

		return NfcConnectorCode.SUCCESS.toByte();
	}


	public synchronized byte[] sendData(byte[] data)
	{
		if (this.nfcAdapter == null)
		{
			return ERROR_NO_NFC_BYTE;
		}

		if (this.mTag == null)
		{
			return ERROR_NO_TAG_BYTE;
		}

		try
		{
			if (!this.mTag.isConnected())
			{
				try
				{
					this.mTag.connect();
				}
				catch (IOException e)
				{
					this.mTag = null;
					return ERROR_CONNECT_BYTE;
				}
			}

			byte[] cardAnswer;
			try
			{
				cardAnswer = this.mTag.transceive(data);
			}
			catch (TagLostException e)
			{
				this.mTag = null;
				return ERROR_TAG_LOST_BYTE;
			}
			catch (IOException e)
			{
				return ERROR_TRANSMIT_BYTE;
			}

			ByteBuffer buffer = ByteBuffer.allocate(1 + cardAnswer.length);
			buffer.put(SUCCESS_BYTE);
			buffer.put(cardAnswer);
			return buffer.array();
		}
		catch (Throwable t)
		{
			Log.e(LOG_TAG, "Cannot send data ", t);
			return ERROR_UNKNOWN_BYTE;
		}
	}


	private static boolean isExtendedLengthApduSupported(IsoDep pTag)
	{
		return pTag.isExtendedLengthApduSupported() && pTag.getMaxTransceiveLength() > 499;
	}


	public synchronized byte getExtendedLengthApduSupportStatus()
	{
		if (mTag == null)
		{
			return ExtendedLengthApduSupportCode.UNKNOWN.toByte();
		}
		return isExtendedLengthApduSupported(mTag) ? ExtendedLengthApduSupportCode.SUPPORTED.toByte() : ExtendedLengthApduSupportCode.NOT_SUPPORTED.toByte();
	}


	public synchronized byte isCardPresent()
	{
		if (this.nfcAdapter == null)
		{
			return NfcConnectorCode.ERROR_NO_NFC.toByte();
		}

		if (this.mTag == null || !isExtendedLengthApduSupported(mTag))
		{
			return NfcCardCode.NO_CARD.toByte();
		}

		if (this.newTag)
		{
			this.newTag = false;

			return NfcCardCode.NEW_CARD.toByte();
		}

		return NfcCardCode.SAME_CARD.toByte();
	}


}
