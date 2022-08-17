/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Style 1.0
import Governikus.View 1.0


Item {
	id: baseItem

	property alias imageSource: image.source
	property alias itemText: text.text
	property alias textStyle: text.textStyle
	property string accessibleText
	property url link
	property string label

	implicitHeight: Math.max(image.implicitHeight, text.implicitHeight)
	implicitWidth: image.implicitWidth + text.implicitWidth + Constants.groupbox_spacing

	Accessible.name: label + " . " + accessibleText
	Accessible.role: Accessible.ListItem

	activeFocusOnTab: true

	Keys.onSpacePressed: Qt.openUrlExternally(link)

	TintableIcon {
		id: image

		anchors {
			left: parent.left
			verticalCenter: parent.verticalCenter
		}

		sourceSize.height: Constants.is_desktop ? Style.dimens.icon_size : Style.dimens.small_icon_size
		tintColor: text.textStyle.textColor
	}

	GText {
		id: text

		anchors {
			left: image.right
			right: parent.right
			top: parent.top
			bottom: parent.bottom
			leftMargin: Constants.groupbox_spacing
		}

		Accessible.ignored: true

		fontSizeMode: Text.Fit
		minimumPixelSize: Style.dimens.hint_font_size
		verticalAlignment: Text.AlignVCenter
		elide: Text.ElideRight

		linkColor: color
	}

	FocusFrame {
		isOnLightBackground: false
	}
}
