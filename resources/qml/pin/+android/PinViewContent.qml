import QtQuick 2.5

import "../global"


Item {
	readonly property int spacing: (height - pinIcon.height - pinHeader.height - pinDesc.height - govButton.height - Utils.dp(40)) / 3

	Image {
		id: pinIcon

		height: parent.height * 0.25
		width: height

		anchors.top: parent.top
		anchors.topMargin: spacing
		anchors.horizontalCenter: parent.horizontalCenter

		fillMode: Image.PreserveAspectFit
		smooth: true
		source: "qrc:///images/icon_Pin.svg"
		sourceSize.height: 256
	}

	Text {
		id: pinHeader

		text: qsTr("PIN Management")

		anchors.top: pinIcon.bottom
		anchors.topMargin: spacing
		anchors.horizontalCenter: parent.horizontalCenter

		font.pixelSize: Constants.header_font_size
		color: Constants.blue
	}

	Text {
		id: pinDesc

		anchors.margins: Utils.dp(10)
		anchors.top: pinHeader.bottom
		anchors.topMargin: Utils.dp(10)
		anchors.horizontalCenter: parent.horizontalCenter

		text: qsTr("You have the opportunity to change your transport PIN into a personal PIN. You can also change the PIN at any time or unblock the PIN using the personal unblocking key (PUK). The transport PIN and the PUK can be found in the letter sent to you by your competent authority.")
		font.pixelSize: Constants.normal_font_size
		horizontalAlignment: Text.AlignHCenter
		width: parent.width - Utils.dp(60)
		wrapMode: Text.WordWrap
	}

	Button {
		id: govButton

		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottomMargin: Utils.dp(30)

		text: qsTr("Change PIN now")
		onClicked: changePinModel.startWorkflow()
	}
}
