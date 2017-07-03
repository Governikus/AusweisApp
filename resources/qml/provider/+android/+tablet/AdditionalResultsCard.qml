import QtQuick 2.6
import QtQuick.Layouts 1.2

import "../../../global"

Rectangle {
	id: baseItem

	readonly property int sidePadding: width * 0.0512
	readonly property int headerHeight: height * 0.4752
	readonly property int infoTextHeight: height * 0.3168
	readonly property int infoTextFontSize: Constants.normal_font_size
	readonly property int footerHeight: height - (headerHeight + infoTextHeight)
	readonly property int borderWidth: 2
	readonly property color titleBarColor: Constants.blue

	property int totalHits: citizenItem.hits + insuranceItem.hits + financeItem.hits + otherItem.hits

	visible: totalHits > 0

	AdditionalResultsItem {
		id: citizenItem

		providerCategory: "citizen"
	}

	AdditionalResultsItem {
		id: insuranceItem

		providerCategory: "insurance"
	}

	AdditionalResultsItem {
		id: financeItem

		providerCategory: "finance"
	}

	AdditionalResultsItem {
		id: otherItem

		providerCategory: "other"
	}

	Column {
		height: baseItem.height
		width: baseItem.width
		anchors.centerIn: parent

		Image {
			id: backgroundImage

			source: Category.backgroundImageSource("all")
			height: baseItem.headerHeight
			width: parent.width
			fillMode: Image.Stretch
			anchors.horizontalCenter: parent.horizontalCenter

			Rectangle {
				id: iconBackground

				anchors.centerIn: icon
				height: icon.height
				width: height
				radius: width * 0.5
				border.width: 0
				color: Constants.all_image_background_color
			}

			Image {
				id: icon
				source: "qrc:///images/All.png"

				anchors.horizontalCenter: backgroundImage.horizontalCenter
				anchors.bottom: backgroundImage.bottom
				anchors.bottomMargin: Utils.dp(20)
				fillMode: Image.PreserveAspectFit
				width: backgroundImage.width * 0.26
				height: width
			}
		}

		Rectangle {
			id: textRectangle

			readonly property int infoTextFontSize: Constants.small_font_size
			height: baseItem.infoTextHeight + baseItem.sidePadding
			width: parent.width

			Text {
				text: '<html>' + qsTr("Additional results: ") + baseItem.totalHits + '</html>'

				anchors.centerIn: parent

				font.bold: true
				font.pixelSize: baseItem.infoTextFontSize
				color: PlatformConstants.dark_grey_secondary_text
			}
		}

		Rectangle {
			height: baseItem.footerHeight
			width: parent.width
			color: baseItem.titleBarColor
		}
	}

	MouseArea {
		anchors.fill: parent

		onClicked: {
			citizenItem.selectCategoryIfMatching()
			insuranceItem.selectCategoryIfMatching()
			financeItem.selectCategoryIfMatching()
			otherItem.selectCategoryIfMatching()
		}
	}
}
