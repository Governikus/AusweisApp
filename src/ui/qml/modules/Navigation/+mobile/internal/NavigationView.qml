/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type

Control {
	id: root

	required property int activeModule
	property int safeAreaBottomMargin: 0

	signal show(int pModule)

	Layout.maximumWidth: Style.dimens.max_text_width
	Layout.minimumWidth: navigationRow.Layout.minimumWidth + leftPadding + rightPadding
	Layout.preferredHeight: Math.ceil(implicitHeight)
	Layout.preferredWidth: contentItem.Layout.preferredWidth + leftPadding + rightPadding
	bottomPadding: Style.dimens.navigation_bar_bottom_padding
	horizontalPadding: Style.dimens.navigation_bar_padding
	topPadding: Style.dimens.navigation_bar_padding

	background: RoundedRectangle {
		bottomLeftCorner: false
		bottomRightCorner: false
		color: Style.color.pane.background.basic_unchecked
		height: parent.height + root.safeAreaBottomMargin
		layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
		radius: Style.dimens.pane_radius

		layer.effect: GDropShadow {
			shadowVerticalOffset: -3
		}
	}
	contentItem: RowLayout {
		id: navigationRow

		readonly property bool horizontalIcons: width >= Layout.preferredWidth

		Layout.preferredWidth: repeater.maxItemWidth * visibleChildren.length + spacing * (visibleChildren.length - 1)

		GRepeater {
			id: repeater

			model: navModel

			delegate: NavigationItem {
				required property string desc
				required property url image
				readonly property var mainViewSubViews: [UiModule.IDENTIFY, UiModule.SELF_AUTHENTICATION, UiModule.PINMANAGEMENT, UiModule.CHECK_ID_CARD]
				required property int module

				Accessible.ignored: Utils.isAccessibleIgnored(root)
				Layout.fillHeight: true
				Layout.fillWidth: true
				Layout.preferredWidth: repeater.maxItemWidth
				checked: root.activeModule === module || (module === UiModule.DEFAULT && mainViewSubViews.includes(root.activeModule))
				count: repeater.count
				flowHorizontally: navigationRow.horizontalIcons
				source: image
				text: qsTr(desc)

				onClicked: {
					root.show(module);
				}
			}
		}
	}

	ListModel {
		id: navModel

		ListElement {
			desc: QT_TR_NOOP("Start")
			image: "qrc:///images/mobile/home.svg"
			module: UiModule.DEFAULT
		}
		ListElement {
			desc: QT_TR_NOOP("Card reader")
			image: "qrc:///images/mobile/phone_card_reader.svg"
			module: UiModule.REMOTE_SERVICE
		}
		ListElement {
			desc: QT_TR_NOOP("Settings")
			image: "qrc:///images/mobile/settings.svg"
			module: UiModule.SETTINGS
		}
		ListElement {
			desc: QT_TR_NOOP("Help")
			image: "qrc:///images/help.svg"
			module: UiModule.HELP
		}
	}
}
