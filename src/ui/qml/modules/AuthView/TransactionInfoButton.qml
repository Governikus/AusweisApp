/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.View

GAbstractButton {
	id: root

	//: ALL_PLATFORMS
	readonly property string a11yDescription: qsTr("Show more information about the transaction.")
	readonly property string a11yName: subheading.text + ". " + contentTextMetrics.elidedText + (showDataNotRequiredText ? ". " + dataNotRequiredText.text : "")
	property alias showDataNotRequiredText: dataNotRequiredText.visible
	property alias transactionText: contentText.text

	Accessible.description: enabled ? Utils.resolveA11yDescription(a11yName, a11yDescription) : ""
	Accessible.name: enabled ? Utils.resolveA11yName(a11yName, a11yDescription) : a11yName
	Accessible.role: enabled ? Accessible.Button : Accessible.StaticText
	enabled: contentText.truncated
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

		ColumnLayout {
			spacing: Style.dimens.text_spacing

			Subheading {
				id: subheading

				Accessible.ignored: true
				//: ALL_PLATFORMS
				text: qsTr("Transactional information")
			}
			GText {
				id: contentText

				Accessible.ignored: true
				elide: Text.ElideRight
				maximumLineCount: 2
				objectName: "transactionText"
				textFormat: Text.StyledText
				visible: !!text

				TextMetrics {
					id: contentTextMetrics

					elide: contentText.elide
					elideWidth: contentText.width * contentText.maximumLineCount
					font: contentText.font
					text: contentText.text
				}
			}
			GText {
				id: dataNotRequiredText

				Accessible.ignored: true
				//: ALL_PLATFORMS
				text: qsTr("The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.")
			}
		}
		TintableIcon {
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
			source: "qrc:///images/material_arrow_right.svg"
			sourceSize.height: Style.dimens.small_icon_size
			tintColor: contentText.color
			visible: root.enabled
		}
	}

	HoverHandler {
		id: hoverHandler

		enabled: root.enabled
	}
	StatefulColors {
		id: colors

		disabledCondition: false
		hoveredCondition: hoverHandler.hovered
		statefulControl: root
	}
}
