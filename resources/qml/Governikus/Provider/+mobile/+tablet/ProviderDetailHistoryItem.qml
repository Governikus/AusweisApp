/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

ListItem {
	id: baseItem

	property var dateTime: ""
	property string infoText: ""
	property var openInfoFunction: function () {}
	property string providerName: ""
	property string providerPostalAddress: ""
	property string purposeText: ""
	property string requestedDataText: ""
	property string termsOfUsageText: ""

	Accessible.description: qsTr("Click to view details of history entry.")
	contentMarginLeft: 0
	contentMarginRight: 0
	//: LABEL ANDROID IOS
	footerText: purposeText !== "" ? purposeText : qsTr("Touch for more details")
	headerText: Utils.historyDateString(dateTime)
	height: 72
	//: LABEL ANDROID IOS
	text: (!!providerName ? providerName : qsTr("Touch for more details"))

	onClicked: baseItem.openInfoFunction({
			"providerName": baseItem.providerName,
			"providerPostalAddress": baseItem.providerPostalAddress,
			"purposeText": baseItem.purposeText,
			"requestedDataText": baseItem.requestedDataText,
			"termsOfUsageText": baseItem.termsOfUsageText
		})
}
