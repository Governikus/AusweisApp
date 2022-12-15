/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0

Item {
	id: baseItem

	property string category: ""
	property alias imageSource: icon.source
	property alias text: label.text

	Accessible.checkable: true
	Accessible.checked: checkBox.checked
	Accessible.name: text
	Accessible.role: Accessible.CheckBox
	anchors.verticalCenter: parent.verticalCenter
	height: parent.height
	width: mainContent.width

	Accessible.onPressAction: mouseArea.clicked(null)

	Row {
		id: mainContent
		anchors.verticalCenter: parent.verticalCenter
		height: parent.height
		spacing: 5

		Image {
			id: icon
			anchors.verticalCenter: parent.verticalCenter
			fillMode: Image.PreserveAspectFit
			height: baseItem.height * 0.7
			width: height
		}
		GText {
			id: label
			Accessible.ignored: true
			anchors.verticalCenter: parent.verticalCenter
		}
		GCheckBox {
			id: checkBox
			Accessible.ignored: true
			anchors.verticalCenter: parent.verticalCenter
			checked: ProviderCategoryFilterModel.categories.indexOf(baseItem.category) !== -1
			visible: true
		}
	}
	MouseArea {
		id: mouseArea
		anchors.fill: parent

		onClicked: ProviderCategoryFilterModel.updateCategorySelection(category, !checkBox.checked)
	}
}
