/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

RowLayout {
	id: root

	property alias buttonIconSource: button.icon.source
	property alias buttonText: button.text
	property alias buttonTooltip: button.enabledTooltipText
	property string description: ""
	property alias iconSource: icon.source
	property alias tintIcon: icon.tintEnabled
	required property string title

	signal clicked

	spacing: Style.dimens.pane_spacing

	TintableIcon {
		id: icon

		sourceSize.width: Style.dimens.icon_size
		tintColor: Style.color.textSubline.basic
	}
	LabeledText {
		id: labeledText

		Layout.fillWidth: true
		label: root.title
		text: root.description

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)
	}
	GButton {
		id: button

		tintIcon: true

		onClicked: root.clicked()
	}
}
