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

	property string accessibleText
	property alias imageSource: image.source
	property alias itemText: text.text
	property string label
	property url link
	property alias textStyle: text.textStyle

	Accessible.name: label + " . " + accessibleText
	Accessible.role: Accessible.ListItem
	activeFocusOnTab: true
	implicitHeight: Math.max(image.implicitHeight, text.implicitHeight)
	implicitWidth: image.implicitWidth + text.implicitWidth + Constants.groupbox_spacing

	Keys.onSpacePressed: Qt.openUrlExternally(link)

	TintableIcon {
		id: image
		sourceSize.height: Constants.is_desktop ? Style.dimens.icon_size : Style.dimens.small_icon_size
		tintColor: text.textStyle.textColor

		anchors {
			left: parent.left
			verticalCenter: parent.verticalCenter
		}
	}
	GText {
		id: text
		Accessible.ignored: true
		elide: Text.ElideRight
		fontSizeMode: Text.Fit
		linkColor: color
		minimumPixelSize: Style.dimens.hint_font_size
		verticalAlignment: Text.AlignVCenter

		anchors {
			bottom: parent.bottom
			left: image.right
			leftMargin: Constants.groupbox_spacing
			right: parent.right
			top: parent.top
		}
	}
	FocusFrame {
		isOnLightBackground: false
	}
}
