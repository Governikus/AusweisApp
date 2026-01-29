/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
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
	property string linkToOpen
	property alias tintIcon: icon.tintEnabled
	required property string title

	signal clicked

	spacing: Style.dimens.pane_spacing

	TintableIcon {
		id: icon

		sourceSize.width: Style.dimens.icon_size
		tintColor: Style.color.textSubline.basic_unchecked
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

		readonly property bool hasLink: root.linkToOpen !== ""

		Accessible.description: hasLink ? Utils.platformAgnosticLinkOpenText(root.linkToOpen, Accessible.name) : ""
		Accessible.name: (root.title && hasLink ? root.title + ", " : "") + text
		Accessible.role: hasLink ? Accessible.Link : Accessible.Button
		tintIcon: true

		onClicked: {
			if (hasLink) {
				Qt.openUrlExternally(root.linkToOpen);
			} else {
				root.clicked();
			}
		}
	}
}
