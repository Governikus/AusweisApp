/*!
 * \brief Unit tests for \ref NotificationModel
 *
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#include "NotificationModel.h"


#include <QDebug>
#include <QtTest>

using namespace governikus;

class test_NotificationModel
	: public QObject
{

	Q_OBJECT
	QSharedPointer<NotificationModel> mModel;

	private Q_SLOTS:
		void init()
		{
			mModel.reset(new NotificationModel());
		}


		void cleanup()
		{
			mModel.clear();
		}


		void test_OnNewLogMsg()
		{
			QSignalSpy spy(mModel.data(), &NotificationModel::fireLastTypeChanged);

			const QString msg("message");
			const QString developermode("developermode");
			for (int i = 0; i < 20; i++)
			{
				mModel->onNewLogMsg(msg, developermode);
				QCOMPARE(mModel->mNotificationEntries.at(i).mText, msg);
				QCOMPARE(mModel->mNotificationEntries.at(i).mType, developermode);
			}
			const QString newMsg("new message");
			const QString feedback("feedback");
			mModel->onNewLogMsg(newMsg, feedback);
			QCOMPARE(mModel->mNotificationEntries.at(20).mText, newMsg);
			QCOMPARE(mModel->mNotificationEntries.at(20).mType, feedback);
			QCOMPARE(spy.count(), 21);
		}


		void test_Data_data()
		{
			QTest::addColumn<int>("row");
			QTest::addColumn<int>("size");
			QTest::addColumn<int>("role");
			QTest::addColumn<QVariant>("output");

			int type = NotificationModel::UserRoles::TYPE;
			int time = NotificationModel::UserRoles::TIME;

			QTest::newRow("entriesEmpty") << 0 << 0 << 0 << QVariant();
			QTest::newRow("RowNumberEqualsSize") << 2 << 2 << type << QVariant();
			QTest::newRow("entriesFirstIndex0Type") << 4 << 5 << type << QVariant(QString("developermode"));
			QTest::newRow("entriesFirstIndex2Text") << 3 << 22 << 5 << QVariant(QString("message"));
			QTest::newRow("indexOutOfRange") << 10 << 5 << time << QVariant();
		}


		void test_Data()
		{
			QFETCH(int, row);
			QFETCH(int, size);
			QFETCH(int, role);
			QFETCH(QVariant, output);

			QModelIndex index = mModel->createIndex(row, 0);
			const QString msg("message");
			const QString developermode("developermode");
			for (int i = 0; i < size; i++)
			{
				mModel->onNewLogMsg(msg, developermode);
			}
			QCOMPARE(mModel->data(index, role), output);
		}


};

QTEST_GUILESS_MAIN(test_NotificationModel)
#include "test_NotificationModel.moc"
