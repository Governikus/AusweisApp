import QtQuick 2.6

import "../global"


Column {
	id: baseItem
	spacing: Constants.pane_spacing

	property string description: ""

	Text {
		font.pixelSize: Constants.header_font_size
		color: PlatformConstants.blue_primary
		text: qsTr("Description")
	}

	Text {
		font.pixelSize: Constants.normal_font_size
		horizontalAlignment: Text.AlignJustify
		text: baseItem.description
		width: parent.width
		wrapMode: Text.Wrap
	}
}
