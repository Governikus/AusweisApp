/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.Type.HistoryModel 1.0

Column {
	id: baseItem

	property var openHistoryInfoFunc: function () {}

	signal scrollHistoryDown
	signal scrollHistoryUp

	spacing: Constants.pane_spacing

	GText {
		id: headerText
		Accessible.name: text
		Accessible.role: Accessible.StaticText

		//: LABEL ANDROID_TABLET IOS_TABLET
		text: qsTr("History")
		textStyle: Style.text.header_accent
	}
	Repeater {
		id: historyItemRepeater
		model: HistoryModel.nameFilter

		ProviderDetailHistoryItem {
			dateTime: model.dateTime
			//: LABEL ANDROID_TABLET IOS_TABLET
			infoText: qsTr("Purpose for reading out requested data")
			openInfoFunction: baseItem.openHistoryInfoFunc
			providerName: subject
			providerPostalAddress: providerPostalAddress
			purposeText: purpose
			requestedDataText: requestedData
			termsOfUsageText: termsOfUsage
			width: parent.width

			Accessible.onScrollDownAction: baseItem.scrollHistoryDown()
			Accessible.onScrollUpAction: baseItem.scrollHistoryUp()
		}
	}
	GText {
		activeFocusOnTab: true

		//: INFO ANDROID_TABLET IOS_TABLET No authentication history, placeholder text.
		text: qsTr("Currently there are no history entries.")
		visible: historyItemRepeater.count === 0
		width: parent.width
	}
}
