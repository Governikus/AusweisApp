/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

Item {
	id: baseItem

	property alias icon: imageItem.source
	property alias iconTintColor: imageItem.tintColor
	property alias text: textItem.text

	signal clicked

	Accessible.focusable: true
	Accessible.ignored: imageItem.source == "" && textItem.text === ""
	Accessible.name: text
	Accessible.role: Accessible.Button
	implicitHeight: contentLayout.implicitHeight
	implicitWidth: contentLayout.implicitWidth

	Accessible.onPressAction: clicked()

	RowLayout {
		id: contentLayout

		anchors.fill: parent
		spacing: 0

		TintableIcon {
			id: imageItem

			Layout.preferredHeight: Layout.preferredWidth
			Layout.preferredWidth: Style.dimens.small_icon_size
			Layout.rightMargin: Constants.text_spacing
			playAnimation: true
			sourceSize.width: width
			tintColor: textItem.color
			visible: imageItem.source != ""
		}
		GCrossBlendedText {
			id: textItem

			Accessible.ignored: true
			Layout.alignment: Qt.AlignVCenter
			color: Style.color.text
			elide: Text.ElideRight
			maximumLineCount: 1
			textStyle: Style.text.navigation
			visible: text !== ""
			wrapMode: Text.NoWrap
		}
	}
	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: baseItem.clicked()
	}
}
