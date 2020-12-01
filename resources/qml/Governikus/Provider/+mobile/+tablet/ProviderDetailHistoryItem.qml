/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0

ListItem {
	id: baseItem

	property string providerName: ""
	property string providerPostalAddress: ""
	property var dateTime: ""
	property string infoText: ""
	property string purposeText: ""
	property string requestedDataText: ""
	property string termsOfUsageText: ""
	property var openInfoFunction: function () {}

	height: 72

	Accessible.description: qsTr("Click to view details of history entry.")

	//: LABEL ANDROID IOS
	headerText: (Utils.isToday(dateTime) ? qsTr("today") :
				 Utils.isYesterday(dateTime) ? qsTr("yesterday") :
				 Utils.isThisWeek(dateTime) ? dateTime.toLocaleString(Qt.locale(), qsTr("dddd")) :
				 dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy"))
				)
	//: LABEL ANDROID IOS
	text: (!!providerName ? providerName : qsTr("Touch for more details"))
	//: LABEL ANDROID IOS
	footerText: purposeText !== "" ? purposeText : qsTr("Touch for more details")
	contentMarginLeft: 0
	contentMarginRight: 0

	onClicked: baseItem.openInfoFunction({
		providerName: baseItem.providerName,
		providerPostalAddress: baseItem.providerPostalAddress,
		purposeText: baseItem.purposeText,
		requestedDataText: baseItem.requestedDataText,
		termsOfUsageText: baseItem.termsOfUsageText
	})
}
