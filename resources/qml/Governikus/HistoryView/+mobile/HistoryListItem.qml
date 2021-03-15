/*
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Type.HistoryModel 1.0

ListItem {
	property var historyModelItem

	height: 72

	Accessible.description: qsTr("Click to view details of history entry.")

	icon: providerIcon !== "" ? providerIcon : (historyModelItem ? Category.imageSource(historyModelItem.providerCategory) : Category.imageSource("unknown"))
	//: LABEL ANDROID IOS
	headerText: (Utils.isToday(dateTime) ? qsTr("today") :
				 Utils.isYesterday(dateTime) ? qsTr("yesterday") :
				 Utils.isThisWeek(dateTime) ? dateTime.toLocaleString(Qt.locale(), qsTr("dddd")) :
				 dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy"))
				)
	text: subject
	//: LABEL ANDROID IOS
	footerText: historyModelItem.purpose !== "" ? historyModelItem.purpose : qsTr("Tap for more details")
}
