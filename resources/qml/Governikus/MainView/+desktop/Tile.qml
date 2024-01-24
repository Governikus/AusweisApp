/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel

RoundedRectangle {
	id: root

	property alias image: image.source
	property alias title: text.text

	signal clicked

	Accessible.name: ApplicationModel.stripHtmlTags(title)
	Accessible.role: Accessible.Button
	borderColor: Style.color.pane_border
	color: mouseArea.pressed ? Style.color.control : Style.color.pane
	layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
	opacity: SettingsModel.showBetaTesting ? 0.9 : 1.0

	layer.effect: GDropShadow {
	}

	Keys.onSpacePressed: clicked()

	MouseArea {
		id: mouseArea

		anchors.fill: parent
		cursorShape: Qt.PointingHandCursor

		onClicked: root.clicked()
	}
	FocusFrame {
	}
	Column {
		spacing: Constants.component_spacing

		anchors {
			left: parent.left
			margins: 2 * Constants.pane_padding
			right: parent.right
			top: parent.top
		}
		TintableIcon {
			id: image

			readonly property int imageHeight: Style.dimens.huge_icon_size

			sourceSize.height: imageHeight
			tintColor: mouseArea.pressed ? Style.color.mainbutton_content_pressed : Style.color.text_subline
		}
		GText {
			id: text

			color: mouseArea.pressed ? Style.color.mainbutton_content_pressed : Style.color.text_title
			horizontalAlignment: Text.AlignLeft
			textStyle: Style.text.navigation
			width: parent.width
		}
	}
}
