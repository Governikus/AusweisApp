/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.SettingsModel 1.0

ListItem {
	property var historyModelItem

	//: LABEL ANDROID IOS
	Accessible.description: qsTr("Click to view details of history entry.")
	//: LABEL ANDROID IOS
	footerText: historyModelItem.purpose !== "" ? historyModelItem.purpose : qsTr("Tap for more details")
	headerText: Utils.historyDateString(dateTime)
	height: 72
	icon: providerIcon !== "" ? providerIcon : (historyModelItem ? Category.imageSource(historyModelItem.providerCategory) : Category.imageSource("unknown"))
	text: subject
}
