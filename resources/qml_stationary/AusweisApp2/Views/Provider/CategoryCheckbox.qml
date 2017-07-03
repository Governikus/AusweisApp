import AusweisApp2.Global 1.0

import QtQuick 2.6
import QtQuick.Layouts 1.2

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
			font.pixelSize: Constants.small_font_size
			anchors.verticalCenter: parent.verticalCenter
		}

		CheckBox {
			id: checkbox
			anchors.verticalCenter: parent.verticalCenter
			visible: true
			checked: providerModel.categories.indexOf(baseItem.category) !== -1

			// TODO: At this time there is no way to select and toggle the checkbox with keyboard.
			//       Depending on the selection model in future the following line is needed.
			// onCheckedChanged: providerModel.updateCategorySelection(category, checked)
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: providerModel.updateCategorySelection(category, !checkbox.checked)
	}
}
