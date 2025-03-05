/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

AbstractButton {
	id: root

	required property string name

	//: LABEL DESKTOP
	Accessible.description: Style.is_layout_desktop ? qsTr("Show more information about the service provider") : ""
	Accessible.name: labelText.label + ". " + labelText.text + (Style.is_layout_desktop ? "" : (". " + link.text))
	Accessible.role: Accessible.Button
	padding: Style.dimens.pane_padding

	background: GPaneBackground {
		Accessible.ignored: true
		border.color: colors.paneBorder
		color: colors.paneBackground

		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: RowLayout {
		spacing: Style.dimens.pane_spacing

		TintableIcon {
			Layout.alignment: Qt.AlignTop
			source: "qrc:///images/info.svg"
			sourceSize.height: Style.dimens.small_icon_size
			tintColor: labelText.labelColor
		}
		ColumnLayout {
			spacing: Style.dimens.pane_spacing

			LabeledText {
				id: labelText

				Accessible.ignored: true
				Layout.fillWidth: true
				bodyElide: Text.ElideRight
				label: Style.is_layout_desktop ?
				//: LABEL DESKTOP
				qsTr("You are about to identify yourself towards the following provider:") :
				//: LABEL ANDROID IOS
				qsTr("Provider")
				maximumBodyLineCount: 1

				//: LABEL ALL_PLATFORMS
				text: root.name !== "" ? root.name : qsTr("See details under \"more...\"")
			}
			GText {
				id: link

				Accessible.ignored: true
				color: labelText.labelColor
				font.pixelSize: UiPluginModel.scaleFactor * 24
				text: Style.is_layout_desktop ?
				//: LABEL DESKTOP
				qsTr("Details about the provider") :
				//: LABEL ANDROID IOS
				qsTr("Touch for more details")
			}
		}
		TintableIcon {
			Layout.alignment: Qt.AlignVCenter
			source: "qrc:///images/material_arrow_right.svg"
			sourceSize.height: Style.dimens.small_icon_size
			tintColor: labelText.textColor
		}
	}

	onFocusChanged: if (focus)
		Utils.positionViewAtItem(this)

	HoverHandler {
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		hoveredCondition: hoverHandler.hovered
		statefulControl: root
	}
}
