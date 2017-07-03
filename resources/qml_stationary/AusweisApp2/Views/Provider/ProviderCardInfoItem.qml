import AusweisApp2.Global 1.0

import QtQuick 2.6


Rectangle {
	id: baseItem

	property alias providerName: subjectText.text
	property alias providerAddressDomain: addressText.text

	Text {
		id: subjectText
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.verticalCenter
		anchors.margins: Utils.dp(15)
		anchors.bottomMargin: 0

		verticalAlignment: Text.AlignVCenter
		elide: Text.ElideRight
	}

	Text {
		id: addressText
		anchors.top: parent.verticalCenter
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: Utils.dp(15)
		anchors.topMargin: 0

		verticalAlignment: Text.AlignVCenter
		font.pixelSize: Constants.small_font_size
		elide: Text.ElideRight
		color: Constants.blue_dark
	}
}

