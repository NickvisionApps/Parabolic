package org.nickvision.tubeconverter.ui

import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.window.application
import org.jetbrains.jewel.foundation.theme.JewelTheme
import org.jetbrains.jewel.foundation.theme.ThemeColorPalette
import org.jetbrains.jewel.foundation.theme.ThemeDefinition
import org.jetbrains.jewel.intui.standalone.theme.IntUiTheme
import org.jetbrains.jewel.intui.standalone.theme.darkThemeDefinition
import org.jetbrains.jewel.intui.standalone.theme.default
import org.jetbrains.jewel.intui.standalone.theme.lightThemeDefinition
import org.jetbrains.jewel.intui.window.decoratedWindow
import org.jetbrains.jewel.intui.window.styling.dark
import org.jetbrains.jewel.intui.window.styling.lightWithLightHeader
import org.jetbrains.jewel.ui.ComponentStyling
import org.jetbrains.jewel.ui.theme.DefaultColorPalette
import org.jetbrains.jewel.window.styling.TitleBarColors
import org.jetbrains.jewel.window.styling.TitleBarStyle

val themeState = mutableIntStateOf(2)

fun main() {
    Controller.init(checkNotNull(System.getProperty("compose.application.resources.dir")))
    themeState.intValue = Controller.getConfiguration().theme
    application(exitProcessOnExit = false) {
        val dark = when (themeState.intValue) {
            0 -> false
            1 -> true
            else -> isSystemInDarkTheme()
        }
        val classic = if (dark) DefaultColorPalette.Dark else DefaultColorPalette.Light
        val islands = if (dark) DefaultColorPalette.IslandsDark else DefaultColorPalette.IslandsLight
        val palette = ThemeColorPalette(
            gray = islands.gray,
            blue = islands.blue,
            green = islands.green,
            red = islands.red,
            yellow = islands.yellow,
            orange = islands.orange,
            purple = islands.purple,
            teal = islands.teal,
            rawMap = classic.rawMap + islands.rawMap,
            isIslands = true,
        )
        val definition = if (dark) {
            JewelTheme.darkThemeDefinition(palette = palette)
        } else {
            JewelTheme.lightThemeDefinition(palette = palette)
        }

        IntUiTheme(
            theme = ThemeDefinition(
                name = definition.name,
                isDark = definition.isDark,
                globalColors = definition.globalColors,
                globalMetrics = definition.globalMetrics,
                defaultTextStyle = definition.defaultTextStyle,
                editorTextStyle = definition.editorTextStyle,
                consoleTextStyle = definition.consoleTextStyle,
                contentColor = definition.contentColor,
                colorPalette = definition.colorPalette,
                iconData = definition.iconData,
                disabledAppearanceValues = definition.disabledAppearanceValues,
                isIslands = true,
            ),
            styling = ComponentStyling.default().decoratedWindow(
                titleBarStyle = if (dark) {
                    TitleBarStyle.dark(
                        colors = TitleBarColors.dark(
                            backgroundColor = Color.Transparent,
                            inactiveBackground = Color.Transparent,
                            borderColor = Color.Transparent,
                        ),
                    )
                } else {
                    val clear = Color.White.copy(alpha = 0f)
                    TitleBarStyle.lightWithLightHeader(
                        colors = TitleBarColors.lightWithLightHeader(
                            backgroundColor = clear,
                            inactiveBackground = clear,
                            borderColor = clear,
                        ),
                    )
                },
            ),
        ) {
            MainWindow(onClose = ::exitApplication)
        }
    }
}
