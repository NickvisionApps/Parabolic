package org.nickvision.tubeconverter.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.DpSize
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.WindowPosition
import androidx.compose.ui.window.rememberWindowState
import org.jetbrains.jewel.foundation.theme.JewelTheme
import org.jetbrains.jewel.window.DecoratedWindow
import org.jetbrains.jewel.window.TitleBar
import org.jetbrains.jewel.window.newFullscreenControls

@Composable
fun SettingsWindow(onClose: () -> Unit) {
    DecoratedWindow(
        onCloseRequest = onClose,
        state = rememberWindowState(size = DpSize(720.dp, 520.dp), position = WindowPosition(Alignment.Center)),
        title = Controller.tr("Settings"),
    ) {
        MatchWindowBackground(window)
        TitleBar(Modifier.newFullscreenControls()) {}

        Box(Modifier.fillMaxSize().background(JewelTheme.globalColors.panelBackground))
    }
}
