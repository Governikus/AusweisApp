import QtQuick 2.5

import "../global"


Item {
	Text {
		id: pinHeader

		text: qsTr("Change Pin")

		anchors.top: parent.top
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.topMargin: Utils.dp(10)

		font.pixelSize: Constants.header_font_size
		color: Constants.blue
	}

	Text {
		id: pinDesc

		width: parent.width * 0.9

		text: qsTr("Here you can change your PIN.<br>We should add some more useful information here! This is just an example to fill a text field.")
		font.pixelSize: Constants.normal_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap

		anchors.top: pinHeader.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.topMargin: Utils.dp(30)
	}

	Image {
		id: pinIcon

		fillMode: Image.PreserveAspectFit
		smooth: true
		source: "qrc:///images/icon_Pin.svg"

		width: parent.width * 2

		anchors.top: pinDesc.bottom
		anchors.bottom: govButton.top
		anchors.left: parent.left
		anchors.right: parent.right

		anchors.margins: 10
	}

	Button {
		id: govButton

		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottomMargin: Utils.dp(30)

		text: qsTr("Change Pin now")
		onClicked: changePinModel.startWorkflow()
	}
}
