@file:OptIn(ExperimentalFoundationApi::class, FlowPreview::class)
@file:Suppress("ktlint:standard:max-line-length")

package org.nickvision.tubeconverter.ui

import androidx.compose.foundation.ExperimentalFoundationApi
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.runtime.snapshotFlow
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.drawBehind
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.input.key.Key
import androidx.compose.ui.input.key.KeyShortcut
import androidx.compose.ui.platform.LocalWindowInfo
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.DpSize
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.MenuBar
import androidx.compose.ui.window.WindowPlacement
import androidx.compose.ui.window.WindowPosition
import androidx.compose.ui.window.rememberWindowState
import kotlinx.coroutines.FlowPreview
import kotlinx.coroutines.flow.debounce
import kotlinx.coroutines.flow.drop
import org.jetbrains.jewel.foundation.theme.JewelTheme
import org.jetbrains.jewel.ui.component.Checkbox
import org.jetbrains.jewel.ui.component.DefaultButton
import org.jetbrains.jewel.ui.component.Icon
import org.jetbrains.jewel.ui.component.IconButton
import org.jetbrains.jewel.ui.component.SelectableIconButton
import org.jetbrains.jewel.ui.component.Text
import org.jetbrains.jewel.ui.component.Tooltip
import org.jetbrains.jewel.ui.icon.IconKey
import org.jetbrains.jewel.ui.icons.AllIconsKeys
import org.jetbrains.jewel.ui.theme.colorPalette
import org.jetbrains.jewel.ui.typography
import org.jetbrains.jewel.window.DecoratedWindow
import org.jetbrains.jewel.window.TitleBar
import org.jetbrains.jewel.window.defaultTitleBarStyle
import org.jetbrains.jewel.window.newFullscreenControls
import java.awt.Desktop
import java.awt.Window
import java.net.URI
import javax.swing.RootPaneContainer

private val GlowCenterX = 140.dp
private val GlowRadius = 720.dp
private val TitleBarBorder = 1.dp
private val IsMac = System.getProperty("os.name").startsWith("Mac")

@Composable
private fun Modifier.frameBackground(top: Dp = 0.dp): Modifier {
    val frame = JewelTheme.colorPalette.rawMap.getValue("grad-g1-amber-bg")
    val glow = JewelTheme.colorPalette.rawMap.getValue("grad-g1-amber-a2")
    return drawBehind {
        val area = size.copy(height = size.height + TitleBarBorder.toPx())
        drawRect(frame, size = area)
        drawRect(
            Brush.radialGradient(
                listOf(glow, Color.Transparent),
                Offset(GlowCenterX.toPx(), -top.toPx()),
                GlowRadius.toPx(),
            ),
            size = area,
        )
    }
}

private enum class Page(val label: () -> String, val icon: IconKey) {
    Downloads({ Controller.tr("Downloads") }, AllIconsKeys.Actions.Download),
    Keyring({ Controller.tr("Keyring") }, AllIconsKeys.Ide.Readonly),
    History({ Controller.tr("History") }, AllIconsKeys.General.History),
}

@Composable
fun MainWindow(onClose: () -> Unit) {
    var settingsOpen by remember { mutableStateOf(false) }
    var page by remember { mutableStateOf(Page.Downloads) }
    var disclaimerOpen by remember { mutableStateOf(false) }

    val geometry = remember { Controller.getConfiguration().windowGeometry }
    val windowState = rememberWindowState(
        size = DpSize(geometry.width.dp, geometry.height.dp),
        position = WindowPosition(geometry.x.dp, geometry.y.dp),
        placement = if (geometry.isMaximized) WindowPlacement.Maximized else WindowPlacement.Floating,
    )
    val saveGeometry = {
        val configuration = Controller.getConfiguration()
        Controller.setConfiguration(
            configuration.copy(
                windowGeometry = if (windowState.placement != WindowPlacement.Floating) {
                    configuration.windowGeometry.copy(isMaximized = true)
                } else {
                    WindowGeometry(
                        height = windowState.size.height.value.toInt(),
                        isMaximized = false,
                        width = windowState.size.width.value.toInt(),
                        x = windowState.position.x.value.toInt(),
                        y = windowState.position.y.value.toInt(),
                    )
                },
            ),
        )
    }
    LaunchedEffect(windowState) {
        snapshotFlow { Triple(windowState.size, windowState.position, windowState.placement) }
            .drop(1)
            .debounce(500)
            .collect { saveGeometry() }
    }

    val close = {
        saveGeometry()
        onClose()
    }

    DecoratedWindow(
        onCloseRequest = close,
        state = windowState,
        title = "Parabolic",
        enabled = !settingsOpen && !disclaimerOpen,
    ) {
        LaunchedEffect(Unit) { disclaimerOpen = Controller.getConfiguration().showDisclaimerOnStartup }
        LaunchedEffect(Unit) {
            if (Desktop.isDesktopSupported() && Desktop.getDesktop().isSupported(Desktop.Action.APP_PREFERENCES)) {
                Desktop.getDesktop().setPreferencesHandler { settingsOpen = true }
            }
        }
        MenuBar {
            Menu(Controller.tr("File")) {
                Item(
                    Controller.tr("Add Download"),
                    shortcut = KeyShortcut(Key.N, ctrl = !IsMac, meta = IsMac),
                    onClick = {},
                )
                if (!IsMac) {
                    Item(Controller.tr("Exit"), onClick = close)
                }
            }
            Menu(Controller.tr("Edit")) {
                if (!IsMac) {
                    Item(
                        Controller.tr("Settings"),
                        shortcut = KeyShortcut(Key.Comma, ctrl = true),
                        onClick = { settingsOpen = true },
                    )
                }
            }
            Menu(Controller.tr("View")) {
                Menu(Controller.tr("Theme")) {
                    listOf(
                        Controller.trp("Theme", "Light"),
                        Controller.trp("Theme", "Dark"),
                        Controller.trp("Theme", "System"),
                    ).forEachIndexed { index, label ->
                        RadioButtonItem(label, selected = themeState.intValue == index, onClick = {
                            themeState.intValue = index
                            Controller.setConfiguration(Controller.getConfiguration().copy(theme = index))
                        })
                    }
                }
            }
            Menu(Controller.tr("Help")) {
                Item(Controller.tr("Check for Updates"), onClick = {})
                Separator()
                Item(
                    Controller.tr("Report a Bug"),
                    onClick = { Desktop.getDesktop().browse(URI(Controller.getIssuesUrl())) },
                )
                Item(
                    Controller.tr("GitHub Repository"),
                    onClick = { Desktop.getDesktop().browse(URI(Controller.getRepoUrl())) },
                )
                Item(
                    Controller.tr("Discussions"),
                    onClick = { Desktop.getDesktop().browse(URI(Controller.getDiscussionsUrl())) },
                )
            }
        }
        MatchWindowBackground(window, JewelTheme.colorPalette.rawMap.getValue("grad-g1-amber-bg"))
        TitleBar(Modifier.newFullscreenControls().frameBackground()) {
            Row(Modifier.align(Alignment.Start).padding(start = 8.dp)) {
                Tooltip({ Text(Controller.tr("Add Download")) }) {
                    IconButton({}, Modifier.size(30.dp)) { _ ->
                        Icon(AllIconsKeys.General.Add, Controller.tr("Add Download"), Modifier.size(20.dp))
                    }
                }
            }
            Row(Modifier.align(Alignment.End).padding(end = 8.dp)) {
                Tooltip({ Text(Controller.tr("Settings")) }) {
                    IconButton({ settingsOpen = true }, Modifier.size(30.dp)) { _ ->
                        Icon(AllIconsKeys.General.Settings, Controller.tr("Settings"), Modifier.size(20.dp))
                    }
                }
            }
        }

        Column(
            Modifier.fillMaxSize()
                .frameBackground(top = JewelTheme.defaultTitleBarStyle.metrics.height + TitleBarBorder),
        ) {
            Row(Modifier.weight(1f).fillMaxWidth()) {
                Column(
                    Modifier.fillMaxHeight().width(46.dp).padding(vertical = 6.dp),
                    horizontalAlignment = Alignment.CenterHorizontally,
                    verticalArrangement = Arrangement.spacedBy(4.dp),
                ) {
                    Page.entries.forEach { entry ->
                        Tooltip({ Text(entry.label()) }) {
                            SelectableIconButton(page == entry, { page = entry }, Modifier.size(34.dp)) { _ ->
                                val blue = page == entry && !JewelTheme.isDark &&
                                    LocalWindowInfo.current.isWindowFocused
                                Icon(
                                    entry.icon,
                                    entry.label(),
                                    Modifier.size(21.dp),
                                    tint = if (blue) Color.White else Color.Unspecified,
                                )
                            }
                        }
                    }
                }
                Box(
                    Modifier.fillMaxSize().padding(end = 4.dp, bottom = 4.dp)
                        .clip(RoundedCornerShape(10.dp))
                        .background(JewelTheme.colorPalette.rawMap.getValue("tool-window-bg")),
                ) {
                    when (page) {
                        Page.Downloads -> DownloadsPage()
                        Page.Keyring -> KeyringPage()
                        Page.History -> HistoryPage()
                    }
                }
            }
            Row(
                Modifier.fillMaxWidth().height(30.dp).padding(start = 12.dp, end = 12.dp, bottom = 8.dp),
                verticalAlignment = Alignment.CenterVertically,
            ) {
                Text(Controller.tr("Ready"), style = JewelTheme.typography.medium)
            }
        }

        if (disclaimerOpen) {
            var dontShowAgain by remember { mutableStateOf(false) }
            val dismiss = {
                if (dontShowAgain) {
                    Controller.setConfiguration(Controller.getConfiguration().copy(showDisclaimerOnStartup = false))
                }
                disclaimerOpen = false
            }
            DecoratedWindow(
                onCloseRequest = dismiss,
                state = rememberWindowState(size = DpSize(480.dp, 280.dp), position = WindowPosition(Alignment.Center)),
                title = Controller.tr("Legal Copyright Disclaimer"),
                resizable = false,
                alwaysOnTop = true,
            ) {
                MatchWindowBackground(window, JewelTheme.colorPalette.rawMap.getValue("dialog-bg"))
                LaunchedEffect(window) { window.toFront() }
                TitleBar(Modifier.newFullscreenControls()) {}
                Column(
                    Modifier.fillMaxSize().padding(20.dp),
                    verticalArrangement = Arrangement.spacedBy(12.dp),
                ) {
                    Text(Controller.tr("Legal Copyright Disclaimer"), fontWeight = FontWeight.Bold)
                    Text(
                        Controller.tr(
                            "Videos on YouTube and other sites may be subject to DMCA protection. The authors of Parabolic do not endorse, and are not responsible for, the use of this application in means that will violate these laws.",
                        ),
                        Modifier.weight(1f),
                    )
                    Row(
                        Modifier.clickable { dontShowAgain = !dontShowAgain },
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(4.dp),
                    ) {
                        Checkbox(dontShowAgain, { dontShowAgain = it })
                        Text(Controller.tr("Don't show this message again"))
                    }
                    Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.End) {
                        DefaultButton(dismiss) {
                            Text(Controller.tr("I understand"))
                        }
                    }
                }
            }
        }

        if (settingsOpen) {
            SettingsWindow(onClose = { settingsOpen = false })
        }
    }
}

@Composable
fun MatchWindowBackground(window: Window, color: Color) {
    LaunchedEffect(window, color) {
        val awtColor = java.awt.Color(color.toArgb())
        window.background = awtColor
        (window as? RootPaneContainer)?.contentPane?.background = awtColor
    }
}
