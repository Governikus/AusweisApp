/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Type

Colors {
	readonly property var disabledPalette: SystemPalette {
		colorGroup: SystemPalette.Disabled
	}
	readonly property var palette: SystemPalette {
		colorGroup: SystemPalette.Active
	}

	background: palette.base
	border: control.background.basic
	fail: "#ff0000"
	focus_indicator: textNormal.basic
	image: "#ff0000"
	progressbar_text: textNormal.basic
	success: "#00ff00"
	warning: "#ff0000"

	control: DefaultControlColors {
		content.basic: palette.buttonText
		content.checked: textNormal.basic
	}
	controlCheckbox: DefaultControlColors {
		background.pressed: textNormal.basic
		content.basic: textNormal.basic
	}
	controlRadiobutton: DefaultControlColors {
		background.pressed: textNormal.basic
		content.basic: textNormal.basic
	}
	controlScrollbar: DefaultControlColors {
		background.basic: textNormal.basic
		background.hovered: palette.highlight
		border.hovered: palette.highlight
		border.pressed: palette.highlight
		content.basic: palette.buttonText
		content.checked: textNormal.basic
	}
	controlSwitch: DefaultControlColors {
		background.pressed: textNormal.basic
		content.basic: textNormal.basic
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
		background.hovered: textNormal.basic
		background.pressed: palette.highlight
		border.basic: "#576164"
	}
	textHeadline: DefaultTextColors {
	}
	textNormal: DefaultTextColors {
		checked: palette.highlightedText
	}
	textSubline: DefaultTextColors {
	}
	textTitle: DefaultTextColors {
	}

	component DefaultControlColors: ControlComponents {
		background: ComponentColors {
			basic: palette.button
			checked: palette.button
			disabled: disabledPalette.button
			hovered: textNormal.basic
			pressed: palette.highlight
		}
		border: ComponentColors {
			basic: textNormal.basic
			checked: textNormal.basic
			disabled: textNormal.disabled
			hovered: palette.base
			pressed: palette.highlightedText
		}
		content: ComponentColors {
			basic: textNormal.basic
			checked: palette.buttonText
			disabled: textNormal.disabled
			hovered: palette.base
			pressed: palette.highlightedText
		}
	}
	component DefaultLinkColors: ComponentColors {
		basic: palette.text
		checked: basic
		disabled: disabledPalette.text
		hovered: palette.highlight
		pressed: palette.highlight
	}
	component DefaultPaneColors: CoreComponents {
		background: ComponentColors {
			basic: palette.base
			checked: palette.highlight
			disabled: disabledPalette.base
			hovered: palette.highlight
			pressed: palette.highlight
		}
		border: ComponentColors {
			basic: textNormal.basic
			checked: basic
			disabled: disabledPalette.text
			hovered: textNormal.basic
			pressed: textNormal.basic
		}
	}
	component DefaultTextColors: ComponentColors {
		basic: palette.text
		checked: basic
		disabled: disabledPalette.text
		hovered: basic
		pressed: basic
	}
}
