import QtQuick 2.6
import QtQuick.Layouts 1.2

import "../../../global"

Item {
	id: baseItem
	height: parent.height
	width: mainContent.width
	anchors.verticalCenter: parent.verticalCenter

	property string category: ""
	property alias imageSource: icon.source
	property alias text: label.text

	Row {
		id: mainContent
		height: parent.height
		spacing: Utils.dp(5)
		anchors.verticalCenter: parent.verticalCenter

		Image {
			id: icon
			height: baseItem.height * 0.7
			width: height
			fillMode: Image.PreserveAspectFit
			anchors.verticalCenter: parent.verticalCenter
		}

		Text {
			id: label
			font.pixelSize: Constants.normal_font_size
			anchors.verticalCenter: parent.verticalCenter
		}

		GCheckBox {
			id: checkbox
			anchors.verticalCenter: parent.verticalCenter
			visible: true
			checked: providerModel.categories.indexOf(baseItem.category) !== -1
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: providerModel.updateCategorySelection(category, !checkbox.checked)
	}
}
