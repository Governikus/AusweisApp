/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel

GButton {
	id: root

	property alias image: icon.source
	property string language
	readonly property bool selected: SettingsModel.language === language

	Accessible.checkable: true
	Accessible.checked: selected
	Layout.maximumWidth: Number.POSITIVE_INFINITY
	cursorShape: selected ? Qt.ArrowCursor : Qt.PointingHandCursor
	padding: Constants.groupbox_spacing

	background: RoundedRectangle {
		color: root.selected ? Style.color.pane_active : Style.color.pane

		FocusFrame {
			borderColor: Style.color.control_border
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: ColumnLayout {
		spacing: Constants.component_spacing

		Image {
			id: icon

			Layout.alignment: Qt.AlignHCenter
			sourceSize.height: Style.dimens.icon_size
		}
		GText {
			Layout.alignment: Qt.AlignHCenter
			color: selected ? Style.color.control_content : Style.color.text
			text: root.text
		}
	}

	onClicked: SettingsModel.language = language
}
