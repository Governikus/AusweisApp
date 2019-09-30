/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0


Item {
	id: root

	function scrollYPositionIntoView(pYposition) {
		var availableFlickableHeight = flickable.height - paneContent.anchors.margins
		var dy = pYposition - flickable.contentY - availableFlickableHeight

		if (dy > 0 || flickable.contentY > 0) {
			flickable.contentY += dy

			if (flickable.contentY < 0)
				flickable.contentY = 0
			else if (flickable.contentY + availableFlickableHeight > flickable.contentHeight)
				flickable.contentY = flickable.contentHeight - availableFlickableHeight
		}
	}

	Accessible.role: Accessible.Heading
	Accessible.name: titleText.text

	property alias title: titleText.text
	property alias content: paneContent
	default property alias children: paneContent.children

	Item {
		id: shadowLayer

		anchors.fill: parent

		layer.enabled: true
		layer.effect: DropShadow {
			radius: 8
			samples: 8
			source: background
			color: Qt.darker(Constants.grey, 1.2)
			verticalOffset: 2
		}
	}

	Rectangle {
		id: background

		anchors.fill: parent

		color: Style.color.background_pane
		radius: Style.dimens.corner_radius
	}

	Item {
		anchors.fill: parent

		clip: true

		Flickable {
			id: flickable

			anchors.fill: parent
			anchors.topMargin: Constants.pane_padding
			anchors.leftMargin: Constants.pane_padding
			anchors.bottomMargin: Constants.pane_padding

			boundsBehavior: Flickable.StopAtBounds
			contentHeight: contentColumn.implicitHeight
			ScrollBar.vertical: ScrollBar {
				policy: size === 1.0 ? ScrollBar.AlwaysOff : ScrollBar.AlwaysOn
			}

			Column {
				id: contentColumn

				anchors.fill: parent
				anchors.rightMargin: Constants.pane_padding

				spacing: Constants.pane_spacing

				GText {
					id: titleText

					visible: text !== ""
					width: parent.width

					textStyle: Style.text.header_accent
					elide: Text.ElideRight

					FocusFrame {
						scope: root
						dynamic: false
						border.color: Constants.black
					}
				}

				Column {
					id: paneContent

					width: parent.width

					spacing: Constants.pane_spacing
				}
			}
		}

		ScrollGradients {
			anchors.fill: parent
			color: background.color
		}
	}

}
