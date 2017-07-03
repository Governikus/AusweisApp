import QtQuick 2.6

import "../global"


Column {
	id: baseItem
	spacing: Constants.pane_spacing

	property var openHistoryInfoFunc: function() {
	}

	readonly property int historyItemHeight: Utils.dp(66)
	readonly property int historyItemMargin: Utils.dp(8)

	Text {
		id: headerText

		font.pixelSize: Constants.header_font_size
		color: PlatformConstants.blue_primary
		text: qsTr("History")
	}

	Repeater {
		model: historyModel.nameFilter

		ProviderDetailHistoryItem {
			itemHeight: baseItem.historyItemHeight
			itemMargin: baseItem.historyItemMargin

			width: parent.width

			providerName: subject
			providerPostalAddress: providerPostalAddress
			dateTime: model.dateTime
			infoText: qsTr("Purpose for reading out requested data")
			purposeText: purpose
			requestedDataText: requestedData
			termsOfUsageText: termsOfUsage

			openInfoFunction: baseItem.openHistoryInfoFunc
		}
	}
}
