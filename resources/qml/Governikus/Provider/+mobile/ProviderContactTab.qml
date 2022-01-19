/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: baseItem

	property alias contactModel: infoList.model

	implicitHeight: infoList.contentHeight + 2 * Constants.component_spacing

	ListView {
		id: infoList

		anchors.fill: parent
		anchors.margins: Constants.component_spacing

		interactive: false

		delegate: Item {
			id: delegateItem

			readonly property var imageSource: Qt.resolvedUrl(model.iconSource)
			//: LABEL ANDROID IOS
			readonly property string itemText: (!!model.text ? model.text : qsTr("Unknown"))
			//: LABEL ANDROID IOS
			readonly property string accessibleText: (!!model.accessibleText ? model.accessibleText : qsTr("Unknown"))
			readonly property url link: model.link
			readonly property string label: qsTranslate("ProviderModelItem", model.label)

			width: parent.width
			height: Math.max(textItem.height, 50)

			Accessible.name: label
			Accessible.description: accessibleText
			Accessible.role: Accessible.ListItem

			Image {
				id: imageItem

				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter
				sourceSize.height: Style.dimens.small_icon_size
				sourceSize.width: Style.dimens.small_icon_size

				source: parent.imageSource
				fillMode: Image.PreserveAspectFit

				ColorOverlay {
					anchors.fill: imageItem
					source: imageItem
					color: Style.color.accent
				}
			}

			GText {
				id: textItem

				anchors.left: imageItem.right
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter

				Accessible.ignored: true

				textFormat: Text.RichText
				text: parent.itemText
				verticalAlignment: Text.AlignVCenter
			}

			MouseArea {
				anchors.fill: delegateItem

				enabled: !!parent.link

				onClicked: Qt.openUrlExternally(parent.link)
			}

			GSeparator {
				visible: index !== infoList.count - 1
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.right: parent.right
			}
		}
	}
}
