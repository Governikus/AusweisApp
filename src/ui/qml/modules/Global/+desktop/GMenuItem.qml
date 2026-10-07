/**
 * Copyright (c) 2019-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

RowLayout {
	id: root

	property alias buttonIconSource: button.icon.source
	property alias buttonText: button.text
	property alias buttonToolTip: button.enabledToolTipText
	property string description: ""
	property alias iconSource: icon.source
	property string linkToOpen
	property alias tintIcon: icon.tintEnabled
	required property string title

	signal clicked

	spacing: Style.dimens.groupbox_spacing

	TintableIcon {
		id: icon

		readonly property int verticalAlignment: height > labeledText.height ? Qt.AlignVCenter : Qt.AlignTop

		Layout.alignment: Qt.AlignLeft | verticalAlignment
		sourceSize.width: Style.dimens.icon_size
		tintColor: Style.color.textSubline.basic_unchecked
	}
	LabeledText {
		id: labeledText

		Layout.fillWidth: true
		label: root.title
		text: root.description
	}
	GButton {
		id: button

		readonly property string a11yName: (root.title && hasLink ? root.title + ", " : "") + text
		readonly property bool hasLink: root.linkToOpen !== ""

		Accessible.name: hasLink ? Utils.platformAgnosticLinkOpenText(root.linkToOpen, a11yName) : a11yName
		Accessible.role: hasLink ? Accessible.Link : Accessible.Button
		Layout.alignment: Qt.AlignRight | Qt.AlignTop
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
