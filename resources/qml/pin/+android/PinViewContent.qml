import QtQuick 2.5

import "../global"


Item {
	Image {
		id: pinIcon

		height: parent.height * 0.5
		width: height

		anchors.top: parent.top
		anchors.horizontalCenter: parent.horizontalCenter

		fillMode: Image.PreserveAspectFit
		smooth: true
		source: "qrc:///images/icon_Pin.svg"
	}

	Text {
		id: pinHeader

		text: qsTr("PIN")

		anchors.top: pinIcon.bottom
		anchors.horizontalCenter: parent.horizontalCenter

		font.pixelSize: Constants.header_font_size
		color: Constants.blue
	}

	Text {
		id: pinDesc

		anchors.top: pinHeader.bottom
		anchors.bottom: govButton.top
		anchors.horizontalCenter: parent.horizontalCenter

		text: qsTr("Here you can change your PIN.<br>We should add some more useful information here! This is just an example to fill a text field.")
		font.pixelSize: Constants.normal_font_size
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		width: parent.width - Utils.dp(60)
		wrapMode: Text.WordWrap
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
