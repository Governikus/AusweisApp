/**
 * Copyright (c) 2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

GPaneBackground {
	id: root

	readonly property int padding: Style.dimens.pane_padding
	required property string text
	required property string title

	implicitHeight: containerCol.implicitHeight
	implicitWidth: containerCol.implicitWidth

	ColumnLayout {
		id: containerCol

		anchors.fill: parent
		spacing: 0

		RowLayout {
			Layout.leftMargin: root.padding
			Layout.rightMargin: root.padding
			Layout.topMargin: root.padding
			spacing: Style.dimens.text_spacing

			TintableIcon {
				source: "qrc:///images/info.svg"
				sourceSize.height: Style.dimens.small_icon_size
				tintColor: hintTitle.color
				tintEnabled: true
			}
			Subheading {
				id: hintTitle

				text: root.title
			}
		}
		GText {
			Layout.bottomMargin: root.padding
			Layout.leftMargin: root.padding
			Layout.rightMargin: root.padding
			Layout.topMargin: Style.dimens.groupbox_spacing
			text: root.text
		}
	}
}
