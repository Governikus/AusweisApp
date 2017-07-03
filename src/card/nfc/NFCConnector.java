package com.governikus.ausweisapp2;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;


import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.nfc.TagLostException;
import android.nfc.tech.IsoDep;
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

	private static NFCConnector mSingleInstance = null;

	private Context mContext = null;

	private NfcAdapter mNfcAdapter = null;

	private boolean mNewTag = false;

	private IsoDep mTag = null;

	private ExtendedLengthApduSupportCode mExtendedLengthApduSupportStatus = ExtendedLengthApduSupportCode.UNKNOWN;


	public static synchronized NFCConnector getInstance(Context context)
	{
		if (mSingleInstance == null)
		{
			Log.d(LOG_TAG, "NfcConnector singleton intialized.");
			mSingleInstance = new NFCConnector(context);
		}

		return mSingleInstance;
	}


	private NFCConnector(Context context)
	{
		super();
		mContext = context;
		mNfcAdapter = NfcAdapter.getDefaultAdapter(mContext);
		start();
	}


	public synchronized void updateNfcTag(Tag inputTag)
	{
		if (inputTag != null&& Arrays.asList(inputTag.getTechList()).contains(IsoDep.class.getName()))
		{
			mTag = IsoDep.get(inputTag);
			mNewTag = true;
			if (mExtendedLengthApduSupportStatus == ExtendedLengthApduSupportCode.UNKNOWN)
			{
				Log.i(LOG_TAG, "IsoDep.isExtendedLengthApduSupported " + mTag.isExtendedLengthApduSupported());
				Log.i(LOG_TAG, "IsoDep.getMaxTransceiveLength " + mTag.getMaxTransceiveLength());
				mExtendedLengthApduSupportStatus = mTag.getMaxTransceiveLength() > 499 ? ExtendedLengthApduSupportCode.SUPPORTED : ExtendedLengthApduSupportCode.NOT_SUPPORTED;
			}
		}
	}


	public synchronized byte start()
	{
		if (mNfcAdapter == null)
		{
			return NfcConnectorCode.ERROR_NO_NFC.toByte();
		}

		return NfcConnectorCode.SUCCESS.toByte();
	}


	public synchronized byte stop()
	{
		if (mTag != null)
		{
			if (mTag.isConnected())
			{
				try
				{
					mTag.close();
				}
				catch (IOException e)
				{
					// nothing
				}
			}
			mTag = null;
		}

		return NfcConnectorCode.SUCCESS.toByte();
	}


	public synchronized boolean connectCard()
	{
		if (mTag == null)
		{
			Log.e(LOG_TAG, "Tag is null");
			return false;
		}
		if (mTag.isConnected())
		{
			Log.e(LOG_TAG, "Card is already connected");
			return false;
		}
		try
		{
			mTag.connect();
			return true;
		}
		catch (IOException e)
		{
			Log.e(LOG_TAG, "Cannot connect card", e);
		}
		return false;
	}


	public synchronized boolean disconnectCard()
	{
		if (mTag == null)
		{
			Log.e(LOG_TAG, "Tag is null");
			return false;
		}
		if (!mTag.isConnected())
		{
			Log.e(LOG_TAG, "Card is not connected");
			return false;
		}
		try
		{
			mTag.close();
			return true;
		}
		catch (IOException e)
		{
			Log.e(LOG_TAG, "Cannot disconnect card", e);
		}
		return false;
	}


	public synchronized boolean isCardConnected()
	{
		return mTag != null&& mTag.isConnected();
	}


	public synchronized byte[] sendData(byte[] data)
	{
		if (mNfcAdapter == null)
		{
			return ERROR_NO_NFC_BYTE;
		}

		if (mTag == null)
		{
			return ERROR_NO_TAG_BYTE;
		}
		if (!mTag.isConnected())
		{
			Log.e(LOG_TAG, "Card is not connected");
			return ERROR_CONNECT_BYTE;
		}

		try
		{
			byte[] cardAnswer;
			try
			{
				cardAnswer = mTag.transceive(data);
			}
			catch (TagLostException e)
			{
				mTag = null;
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


	public synchronized byte getExtendedLengthApduSupportStatus()
	{
		return mExtendedLengthApduSupportStatus.toByte();
	}


	public synchronized byte isCardPresent()
	{
		if (mNfcAdapter == null)
		{
			return NfcConnectorCode.ERROR_NO_NFC.toByte();
		}

		if (mTag == null)
		{
			return NfcCardCode.NO_CARD.toByte();
		}

		if (!mTag.isConnected())
		{
			try
			{
				mTag.connect();
				if (!mTag.isConnected())
				{
					return NfcCardCode.NO_CARD.toByte();
				}
				mTag.close();
			}
			catch (IOException e)
			{
				return NfcCardCode.NO_CARD.toByte();
			}
		}

		if (!mNewTag)
		{
			return NfcCardCode.SAME_CARD.toByte();
		}
		mNewTag = false;

		if (!mTag.isExtendedLengthApduSupported())
		{
			Log.i(LOG_TAG, "New NFC tag found with insufficient characteristics " + mTag.getTag().toString());
			return NfcCardCode.NO_CARD.toByte();
		}

		Log.i(LOG_TAG, "New NFC tag found with sufficient characteristics " + mTag.getTag().toString());
		return NfcCardCode.NEW_CARD.toByte();
	}


}
