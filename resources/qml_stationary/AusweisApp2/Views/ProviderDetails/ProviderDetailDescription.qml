import AusweisApp2.Global 1.0

import QtQuick 2.6
import QtQuick.Controls 1.4


ScrollView {
	id: baseItem
	property string description: ""

	horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
	verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

	Column {
		spacing: Constants.pane_spacing

		Text {
//			font.pixelSize: Constants.header_font_size
			color: PlatformConstants.blue_primary
			text: qsTr("Description") + settingsModel.translationTrigger
		}

		Text {
//			font.pixelSize: Constants.normal_font_size
			text: baseItem.description
			width: baseItem.width - Utils.dp(50)
			wrapMode: Text.Wrap
		}
	}
}
