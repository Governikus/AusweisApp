import QtQuick 2.5
import QtQuick.Window 2.2

import Governikus.Global 1.0
import Governikus.TitleBar 1.0

SectionPage {
	id: baseItem
	leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: baseItem.clicked() }

	property alias text: resultText.text
	property bool isError: false
	signal clicked

	Rectangle {
		anchors.fill: parent
		color: Constants.background_color
	}

	// See: http://tagasks.com/in_qtqml_how_to_load_different_images_for_different_device_densities_android
	property int ppi: Screen.pixelDensity * 25.4

	Image {
		id: resultImage
		source: isError ? "qrc:///images/rotes_X.svg" : "qrc:///images/gruener_Haken.svg"
		height: Utils.dp(100)
		width: height
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(60)
		fillMode: Image.PreserveAspectFit
	}

	Text {
		id: resultText
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: resultImage.bottom
		anchors.bottom: button.top

		font.pixelSize: Constants.header_font_size
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		wrapMode: Text.WordWrap
		color: isError ? Constants.red : Constants.blue
		onLinkActivated: Qt.openUrlExternally(link)
	}

	GButton {
		id: button
		width: Utils.dp(90)
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottomMargin: Utils.dp(30)

		text: qsTr("OK") + settingsModel.translationTrigger
		onClicked: baseItem.clicked()
	}
}
