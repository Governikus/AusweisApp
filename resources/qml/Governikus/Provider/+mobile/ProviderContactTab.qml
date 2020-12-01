/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
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

			width: parent.width
			height: Math.max(textItem.height, 50)

			Accessible.role: Accessible.ListItem
			Accessible.name: ApplicationModel.stripHtmlTags(textItem.text)

			Image {
				id: imageItem


				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter
				sourceSize.height: Style.dimens.small_icon_size
				sourceSize.width: Style.dimens.small_icon_size

				source: Qt.resolvedUrl(model.iconSource)
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
				text: !!model.text ? model.text : qsTr("Unknown")
				verticalAlignment: Text.AlignVCenter
			}

			MouseArea {
				anchors.fill: delegateItem

				enabled: !!model.link

				onClicked: Qt.openUrlExternally(model.link)
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
