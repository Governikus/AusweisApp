/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
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
	border: control.background.basic
	error: "#ff0000"
	focus_indicator: textNormal.basic
	image: "#ff0000"
	progressbar_text: textNormal.basic
	success: "#00ff00"
	warning: "#ff0000"

	control: DefaultControlColors {
		content.basic: root.palette.buttonText
		content.checked: root.textNormal.basic
	}
	controlCheckbox: DefaultControlColors {
		background.pressed: root.textNormal.basic
		content.basic: root.textNormal.basic
	}
	controlOptional: DefaultControlColors {
		content.basic: root.palette.buttonText
		content.checked: root.textNormal.basic
	}
	controlRadiobutton: DefaultControlColors {
		background.pressed: root.textNormal.basic
		content.basic: root.textNormal.basic
	}
	controlScrollbar: DefaultControlColors {
		background.basic: root.textNormal.basic
		background.hovered: root.palette.highlight
		border.hovered: root.palette.highlight
		border.pressed: root.palette.highlight
		content.basic: root.palette.buttonText
		content.checked: root.textNormal.basic
	}
	controlStagedProgressBar: DefaultControlColors {
		background.basic: root.background
		border.basic: content.basic
	}
	controlSwitch: DefaultControlColors {
		background.pressed: root.textNormal.basic
		content.basic: root.textNormal.basic
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
		background.hovered: root.textNormal.basic
		background.pressed: root.palette.highlight
		border.basic: "#576164"
	}
	remoteIndicator: ComponentColors {
		basic: "#ff0000"
		checked: basic
		disabled: "#f6b1b1"
		hovered: basic
		pressed: basic
	}
	textHeadline: DefaultTextColors {
	}
	textNormal: DefaultTextColors {
		checked: root.palette.highlightedText
	}
	textSubline: DefaultTextColors {
	}
	textTitle: DefaultTextColors {
	}

	component DefaultControlColors: ControlComponents {
		background: ComponentColors {
			basic: root.palette.button
			checked: root.palette.button
			disabled: root.disabledPalette.button
			hovered: root.textNormal.basic
			pressed: root.palette.highlight
		}
		border: ComponentColors {
			basic: root.textNormal.basic
			checked: root.textNormal.basic
			disabled: root.textNormal.disabled
			hovered: root.palette.base
			pressed: root.palette.highlightedText
		}
		content: ComponentColors {
			basic: root.textNormal.basic
			checked: root.palette.buttonText
			disabled: root.textNormal.disabled
			hovered: root.palette.base
			pressed: root.palette.highlightedText
		}
	}
	component DefaultLinkColors: ComponentColors {
		basic: root.palette.text
		checked: basic
		disabled: root.disabledPalette.text
		hovered: root.palette.highlight
		pressed: root.palette.highlight
	}
	component DefaultPaneColors: CoreComponents {
		background: ComponentColors {
			basic: root.palette.base
			checked: root.palette.highlight
			disabled: root.disabledPalette.base
			hovered: root.palette.highlight
			pressed: root.palette.highlight
		}
		border: ComponentColors {
			basic: root.textNormal.basic
			checked: basic
			disabled: root.disabledPalette.text
			hovered: root.textNormal.basic
			pressed: root.textNormal.basic
		}
	}
	component DefaultTextColors: ComponentColors {
		basic: root.palette.text
		checked: basic
		disabled: root.disabledPalette.text
		hovered: basic
		pressed: basic
	}
}
