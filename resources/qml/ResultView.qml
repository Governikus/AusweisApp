import QtQuick 2.5

import "global"

SectionPage {
	id: baseItem
	leftTitleBarAction: TitleBarMenuAction { state: "hidden" }

	property alias text: resultText.text
	property bool isError: false
	signal clicked

	Rectangle {
		anchors.fill: parent
		color: Constants.background_color
	}


	Image {
		id: resultImage
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.verticalCenter
		fillMode: Image.PreserveAspectFit
		source: isError ? "qrc:///images/rotes_X.svg" : "qrc:///images/gruener_Haken.svg"
		width: Utils.dp(160)
	}
	Text {
		id: resultText
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: resultImage.bottom
		width: parent.width * 0.9
		font.pixelSize: Constants.header_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
		color: Constants.blue
		onLinkActivated: Qt.openUrlExternally(link)
	}

	Button {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Utils.dp(30)
		text: qsTr("Ok")
		onClicked: baseItem.clicked()
	}
}
