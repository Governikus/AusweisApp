/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.UiModule
import Governikus.Type.WorkflowModel

GControl {
	id: root

	Layout.maximumWidth: Math.max(Layout.preferredWidth, Style.dimens.max_text_width)
	Layout.minimumWidth: navigationRow.Layout.minimumWidth + leftPadding + rightPadding
	Layout.preferredWidth: contentItem.Layout.preferredWidth + leftPadding + rightPadding
	bottomInset: -plugin.safeAreaMargins.bottom
	bottomPadding: Style.dimens.navigation_bar_bottom_padding
	horizontalPadding: Style.dimens.navigation_bar_padding
	topPadding: Style.dimens.navigation_bar_padding

	background: RoundedRectangle {
		bottomLeftCorner: false
		bottomRightCorner: false
		color: Style.color.pane
		layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
		radius: Style.dimens.pane_radius

		layer.effect: GDropShadow {
			verticalOffset: -3
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
				readonly property var mainViewSubViews: [UiModule.IDENTIFY, UiModule.SELF_AUTHENTICATION, UiModule.PINMANAGEMENT, UiModule.CHECK_ID_CARD, UiModule.SMART_EID]

				Accessible.ignored: root.Accessible.ignored
				Layout.fillHeight: true
				Layout.fillWidth: true
				Layout.preferredWidth: repeater.maxItemWidth
				flowHorizontally: navigationRow.horizontalIcons
				selected: navigation.activeModule === module || (module === UiModule.DEFAULT && mainViewSubViews.includes(navigation.activeModule))
				source: image
				text: qsTr(desc)

				onClicked: {
					navigation.resetContentArea();
					navigation.show(module);
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
			image: "qrc:///images/mobile/help.svg"
			module: UiModule.HELP
		}
	}
}
