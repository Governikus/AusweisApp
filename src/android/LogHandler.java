/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.LinkedList;
import java.util.List;
import java.util.logging.Handler;
import java.util.logging.LogRecord;
import java.util.logging.Logger;


public class LogHandler extends Handler
{
	private static Logger mRootLogger;

	static
	{
		mRootLogger = Logger.getLogger(BuildConfig.ROOT_LOGGER); // initialize if non-existing and hold strong reference
		for (Handler handler : mRootLogger.getHandlers())
		{
			mRootLogger.removeHandler(handler);
		}
		mRootLogger.addHandler(new LogHandler());
	}

	private static boolean mInitialized;

	private static List<LogRecord> mLogShelve = new LinkedList<>();

	private static native void propagate(int pLevel, String pCategory, String pFile, String pFunction, String pMsg);

	private static void propagate(LogRecord pRecord)
	{
		String msg = pRecord.getMessage();

		if (pRecord.getThrown() != null)
		{
			StringWriter buffer = new StringWriter();
			buffer.write(msg);
			buffer.write("\n");
			pRecord.getThrown().printStackTrace(new PrintWriter(buffer));
			msg = buffer.toString();
		}

		propagate(pRecord.getLevel().intValue(), pRecord.getLoggerName(), pRecord.getSourceClassName(), pRecord.getSourceMethodName(), msg);
	}


	public static synchronized void init()
	{
		if (!mInitialized)
		{
			mInitialized = true;
			for (LogRecord msg : mLogShelve)
			{
				propagate(msg);
			}
			mLogShelve.clear();
		}
	}


	public static Logger getLogger()
	{
		return Logger.getLogger("java");
	}


	public void publish(LogRecord pRecord)
	{
		if (mInitialized)
		{
			propagate(pRecord);
		}
		else
		{
			mLogShelve.add(pRecord);
		}
	}


	public void close()
	{
		// abstract method
	}


	public void flush()
	{
		// abstract method
	}


}
