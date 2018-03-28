import QtQuick 2.6

import Governikus.Global 1.0


Column {
	id: baseItem
	spacing: Constants.pane_spacing

	property string description: ""

	Text {
		font.pixelSize: Constants.header_font_size
		color: Constants.blue
		text: qsTr("Description") + settingsModel.translationTrigger
	}

	Text {
		color: Constants.secondary_text
		font.pixelSize: Constants.normal_font_size
		horizontalAlignment: Text.AlignLeft
		text: baseItem.description
		width: parent.width
		wrapMode: Text.Wrap
	}
}
