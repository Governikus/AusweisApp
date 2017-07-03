import QtQuick 2.5
import QtQuick.Window 2.2

import "global"

SectionPage {
	id: baseItem
	leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: baseItem.clicked() }

	property alias text: resultText.text
	property bool isError: false
	signal clicked

	Rectangle {
		anchors.fill: parent
		color: Constants.background_color
	}

	// See: http://tagasks.com/in_qtqml_how_to_load_different_images_for_different_device_densities_android
	property int ppi: Screen.pixelDensity * 25.4

	readonly property string imageDir: {
		if (ppi >= 360)
			"xxxhdpi"
		else if (ppi >= 270)
			"xxhdpi"
		else if (ppi >= 180)
			"xhdpi"
		else if (ppi >= 135)
			"hdpi"
		else
			"mdpi"
	}

	Image {
		id: resultImage
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(60)

		fillMode: Image.PreserveAspectFit

		source: isError ? "qrc:///images/iOS/rotes_X.svg" : "qrc:///images/android/" + imageDir + "/haken.png"
		width: Utils.dp(160)
	}

	Text {
		id: resultText
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter

		width: parent.width * 0.9

		font.pixelSize: Constants.header_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
		color: Constants.blue
	}

	Button {
		width: Utils.dp(90)

		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottomMargin: Utils.dp(30)

		text: qsTr("Ok")

		onClicked: baseItem.clicked()
	}
}
