/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

Colors {
	id: root

	readonly property var disabledPalette: SystemPalette {
		colorGroup: SystemPalette.Disabled
	}
	readonly property var palette: SystemPalette {
		colorGroup: SystemPalette.Active
	}

	background: palette.base
	border: control.background.basic_unchecked
	error: "#ff0000"
	focus_indicator: textNormal.basic_unchecked
	image: "#ff0000"
	success: "#00ff00"
	warning: "#ff0000"

	control: DefaultControlColors {
		content.basic_checked: root.textNormal.basic_unchecked
		content.basic_unchecked: root.palette.buttonText
	}
	controlCheckbox: DefaultControlColors {
		background.pressed_checked: root.textNormal.basic_unchecked
		background.pressed_unchecked: root.textNormal.basic_unchecked
		content.basic_unchecked: root.textNormal.basic_unchecked
	}
	controlOptional: DefaultControlColors {
		content.basic_checked: root.textNormal.basic_unchecked
		content.basic_unchecked: root.palette.buttonText
	}
	controlRadiobutton: DefaultControlColors {
		background.basic_checked: root.palette.highlight
		background.hovered_checked: background.basic_checked
		background.hovered_unchecked: background.basic_checked
		background.pressed_checked: background.hovered_unchecked
		background.pressed_unchecked: background.hovered_unchecked
		border.basic_checked: background.basic_checked
		border.basic_unchecked: root.palette.buttonText
		border.hovered_checked: border.basic_checked
		border.hovered_unchecked: border.basic_checked
		border.pressed_checked: border.hovered_unchecked
		border.pressed_unchecked: border.hovered_unchecked
		content.basic_checked: root.palette.highlightedText
		content.basic_unchecked: border.basic_unchecked
		content.hovered_checked: content.basic_checked
		content.hovered_unchecked: content.basic_checked
		content.pressed_checked: content.hovered_unchecked
		content.pressed_unchecked: content.hovered_unchecked
	}
	controlScrollbar: DefaultControlColors {
		background.basic_unchecked: root.textNormal.basic_unchecked
		background.hovered_checked: root.palette.highlight
		background.hovered_unchecked: root.palette.highlight
		border.hovered_checked: root.palette.highlight
		border.hovered_unchecked: root.palette.highlight
		border.pressed_checked: root.palette.highlight
		border.pressed_unchecked: root.palette.highlight
		content.basic_checked: root.textNormal.basic_unchecked
		content.basic_unchecked: root.palette.buttonText
	}
	controlStagedProgressBar: DefaultControlColors {
		background.basic_unchecked: root.background
		border.basic_unchecked: content.basic_unchecked
	}
	controlSwitch: DefaultControlColors {
		background.pressed_checked: root.textNormal.basic_unchecked
		background.pressed_unchecked: root.textNormal.basic_unchecked
		content.basic_unchecked: root.textNormal.basic_unchecked
	}
	linkBasic: DefaultLinkColors {
	}
	linkNavigation: DefaultLinkColors {
	}
	linkTitle: DefaultLinkColors {
	}
	pane: DefaultPaneColors {
	}
	paneSublevel: DefaultPaneColors {
		background.hovered_checked: root.textNormal.basic_unchecked
		background.hovered_unchecked: root.textNormal.basic_unchecked
		background.pressed_checked: root.palette.highlight
		background.pressed_unchecked: root.palette.highlight
		border.basic_unchecked: "#576164"
	}
	remoteIndicator: ComponentColors {
		basic_checked: basic_unchecked
		basic_unchecked: "#ff0000"
		disabled_checked: "#f6b1b1"
		disabled_unchecked: "#f6b1b1"
		hovered_checked: basic_unchecked
		hovered_unchecked: basic_unchecked
		pressed_checked: basic_unchecked
		pressed_unchecked: basic_unchecked
	}
	textHeadline: DefaultTextColors {
	}
	textNormal: DefaultTextColors {
		basic_checked: root.palette.highlightedText
	}
	textSubline: DefaultTextColors {
	}
	textTitle: DefaultTextColors {
	}

	component DefaultControlColors: ControlComponents {
		background: ComponentColors {
			basic_checked: root.palette.button
			basic_unchecked: root.palette.button
			disabled_checked: root.disabledPalette.button
			disabled_unchecked: root.disabledPalette.button
			hovered_checked: root.textNormal.basic_unchecked
			hovered_unchecked: root.textNormal.basic_unchecked
			pressed_checked: root.palette.highlight
			pressed_unchecked: root.palette.highlight
		}
		border: ComponentColors {
			basic_checked: root.textNormal.basic_unchecked
			basic_unchecked: root.textNormal.basic_unchecked
			disabled_checked: root.textNormal.disabled_unchecked
			disabled_unchecked: root.textNormal.disabled_unchecked
			hovered_checked: root.palette.base
			hovered_unchecked: root.palette.base
			pressed_checked: root.palette.highlightedText
			pressed_unchecked: root.palette.highlightedText
		}
		content: ComponentColors {
			basic_checked: root.palette.buttonText
			basic_unchecked: root.textNormal.basic_unchecked
			disabled_checked: root.textNormal.disabled_unchecked
			disabled_unchecked: root.textNormal.disabled_unchecked
			hovered_checked: root.palette.base
			hovered_unchecked: root.palette.base
			pressed_checked: root.palette.highlightedText
			pressed_unchecked: root.palette.highlightedText
		}
	}
	component DefaultLinkColors: ComponentColors {
		basic_checked: basic_unchecked
		basic_unchecked: root.palette.text
		disabled_checked: root.disabledPalette.text
		disabled_unchecked: root.disabledPalette.text
		hovered_checked: root.palette.highlight
		hovered_unchecked: root.palette.highlight
		pressed_checked: root.palette.highlight
		pressed_unchecked: root.palette.highlight
	}
	component DefaultPaneColors: CoreComponents {
		background: ComponentColors {
			basic_checked: root.palette.highlight
			basic_unchecked: root.palette.base
			disabled_checked: root.disabledPalette.base
			disabled_unchecked: root.disabledPalette.base
			hovered_checked: root.palette.highlight
			hovered_unchecked: root.palette.highlight
			pressed_checked: root.palette.highlight
			pressed_unchecked: root.palette.highlight
		}
		border: ComponentColors {
			basic_checked: basic_unchecked
			basic_unchecked: root.textNormal.basic_unchecked
			disabled_checked: root.disabledPalette.text
			disabled_unchecked: root.disabledPalette.text
			hovered_checked: root.textNormal.basic_unchecked
			hovered_unchecked: root.textNormal.basic_unchecked
			pressed_checked: root.textNormal.basic_unchecked
			pressed_unchecked: root.textNormal.basic_unchecked
		}
	}
	component DefaultTextColors: ComponentColors {
		basic_checked: basic_unchecked
		basic_unchecked: root.palette.text
		disabled_checked: root.disabledPalette.text
		disabled_unchecked: root.disabledPalette.text
		hovered_checked: basic_unchecked
		hovered_unchecked: basic_unchecked
		pressed_checked: basic_unchecked
		pressed_unchecked: basic_unchecked
	}
}
