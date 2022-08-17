/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
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

	Accessible.role: Accessible.Grouping
	Accessible.name: titleText.text
	Accessible.focusable: true

	property alias title: titleText.text
	property alias content: paneContent
	default property alias data: paneContent.data

	implicitHeight: flickable.contentHeight + 2 * Constants.pane_padding

	Rectangle {
		id: background

		anchors.fill: parent

		color: Style.color.background_pane
		radius: Style.dimens.corner_radius
		border.width: Style.dimens.high_contrast_item_border
		border.color: Style.color.high_contrast_item_border
	}

	Item {
		anchors.fill: parent
		anchors.margins: Style.dimens.high_contrast_item_border

		clip: true

		GFlickable {
			id: flickable

			anchors.fill: parent
			anchors.topMargin: Constants.pane_padding
			anchors.leftMargin: Constants.pane_padding
			anchors.bottomMargin: Constants.pane_padding

			contentHeight: contentColumn.implicitHeight

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
			anchors.margins: Style.dimens.high_contrast_item_border
			color: background.color
		}
	}

}
