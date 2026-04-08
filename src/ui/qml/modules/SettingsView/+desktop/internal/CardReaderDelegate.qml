/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

RoundedRectangle {
	id: root

	property int iconHeight: UiPluginModel.scaleFactor * 105
	required property string readerHTMLDescription
	required property url readerImagePath
	required property bool readerInstalled
	required property string readerName
	required property bool readerSupported
	property bool showInstalledSupportedIcon: true

	//: DESKTOP Text read by screen reader if the text contains a web link to a card reader driver which may be opened.
	Accessible.name: readerName + ". " + ApplicationModel.stripHtmlTags(readerHTMLDescription) + ". " + (textDescription.hasLink ? qsTr("Press space to open link.") : "")
	Accessible.role: textDescription.hasLink ? Accessible.Button : Accessible.ListItem
	activeFocusOnTab: textDescription.hasLink
	color: Style.color.paneSublevel.background.basic_unchecked
	implicitHeight: rowLayout.implicitHeight
	implicitWidth: rowLayout.implicitWidth

	Keys.onEnterPressed: textDescription.tryActivateLink()
	Keys.onReturnPressed: textDescription.tryActivateLink()
	Keys.onSpacePressed: textDescription.tryActivateLink()

	FocusFrame {
	}
	RowLayout {
		id: rowLayout

		anchors.fill: parent
		spacing: Style.dimens.pane_spacing

		RoundedRectangle {
			Layout.fillHeight: true
			Layout.preferredHeight: root.iconHeight + 2 * Style.dimens.pane_padding
			Layout.preferredWidth: root.iconHeight + Style.dimens.pane_padding
			bottomRightCorner: false
			topRightCorner: false

			gradient: Gradient {
				orientation: Gradient.Horizontal

				GradientStop {
					color: Style.color.paneSublevel.background.basic_unchecked
					position: 0
				}
				GradientStop {
					color: Style.color.pane.background.basic_unchecked
					position: 1
				}
			}

			Image {
				anchors.centerIn: parent
				asynchronous: true
				fillMode: Image.PreserveAspectFit
				height: root.iconHeight * 0.95
				source: root.readerImagePath
				width: root.iconHeight * 0.95
			}
		}
		ColumnLayout {
			Layout.alignment: Qt.AlignLeft
			Layout.bottomMargin: Style.dimens.pane_padding
			Layout.leftMargin: 0
			Layout.maximumWidth: Number.POSITIVE_INFINITY
			Layout.topMargin: Style.dimens.pane_padding
			spacing: Style.dimens.text_spacing

			GText {
				Accessible.ignored: true
				Layout.alignment: Qt.AlignLeft
				clip: true
				text: root.readerName
				textStyle: Style.text.headline
			}
			GText {
				id: textDescription

				Accessible.ignored: true
				Layout.alignment: Qt.AlignLeft
				text: root.readerHTMLDescription
				visible: text !== ""
			}
		}
		StatusAnimation {
			Layout.rightMargin: Style.dimens.pane_padding
			sourceSize.height: root.iconHeight * 0.33
			symbol.type: {
				if (root.readerInstalled) {
					if (root.readerSupported) {
						return Symbol.Type.CHECK;
					}
					return Symbol.Type.WARNING;
				}
				return Symbol.Type.ERROR;
			}
			visible: root.showInstalledSupportedIcon
		}
	}
}
