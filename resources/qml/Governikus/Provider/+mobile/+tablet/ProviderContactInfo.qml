/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


Rectangle {
	id: baseItem
	property alias contactModel: contactListView.model

	onWidthChanged: info.resetSize()
	onHeightChanged: info.resetSize()

	Column {
		id: info
		width: parent.width

		property int sizeRecudctor: 0
		function resetSize() {
			if (childrenRect.height < baseItem.height) {
				sizeRecudctor = 0
			}
		}
		function approximateSize() {
			if (childrenRect.height > baseItem.height) {
				sizeRecudctor = sizeRecudctor +1
			}
		}

		onWidthChanged: info.approximateSize()
		onHeightChanged: info.approximateSize()
		onVisibleChanged: { info.resetSize(); info.approximateSize() }
		onChildrenRectChanged: info.approximateSize()

		GText {
			//: LABEL ANDROID_TABLET IOS_TABLET
			text: qsTr("Contact") + SettingsModel.translationTrigger
			textStyle: Style.text.header_inverse
			padding: Constants.component_spacing
		}
		Rectangle {
			// The delegates have space in between which is
			// therefore the color of this very Rectangle.
			anchors.left: parent.left
			anchors.right: parent.right
			height: contactListView.height
			color: Constants.white

			ListView {
				id: contactListView
				width: parent.width
				height: contentHeight
				interactive: false
				spacing: 2
				delegate:  ProviderContactInfoItem {
					anchors.left: parent.left
					width: contactListView.width
					color: baseItem.color
					imageSource: Qt.resolvedUrl(model.iconSource)
					//: LABEL ANDROID_TABLET IOS_TABLET
					itemText: (!!model.text ? model.text : qsTr("Unknown")) + SettingsModel.translationTrigger
					link: model.link
					sizeRecudctor: info.sizeRecudctor
				}
			}
		}
	}
}
