@file:Suppress("ktlint:standard:max-line-length")

package org.nickvision.tubeconverter.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.input.rememberTextFieldState
import androidx.compose.foundation.verticalScroll
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.key
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberUpdatedState
import androidx.compose.runtime.setValue
import androidx.compose.runtime.snapshotFlow
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.focus.onFocusChanged
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.input.pointer.isCtrlPressed
import androidx.compose.ui.input.pointer.isMetaPressed
import androidx.compose.ui.input.pointer.isShiftPressed
import androidx.compose.ui.platform.LocalWindowInfo
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.DpSize
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.WindowPosition
import androidx.compose.ui.window.rememberWindowState
import kotlinx.coroutines.flow.drop
import org.jetbrains.jewel.foundation.theme.JewelTheme
import org.jetbrains.jewel.ui.Orientation
import org.jetbrains.jewel.ui.Outline
import org.jetbrains.jewel.ui.component.Checkbox
import org.jetbrains.jewel.ui.component.Divider
import org.jetbrains.jewel.ui.component.ExternalLink
import org.jetbrains.jewel.ui.component.HorizontalSplitLayout
import org.jetbrains.jewel.ui.component.Icon
import org.jetbrains.jewel.ui.component.IconButton
import org.jetbrains.jewel.ui.component.InfoText
import org.jetbrains.jewel.ui.component.ListComboBox
import org.jetbrains.jewel.ui.component.OutlinedButton
import org.jetbrains.jewel.ui.component.SearchTextField
import org.jetbrains.jewel.ui.component.SimpleListItem
import org.jetbrains.jewel.ui.component.Text
import org.jetbrains.jewel.ui.component.TextField
import org.jetbrains.jewel.ui.component.VerticallyScrollableContainer
import org.jetbrains.jewel.ui.component.rememberSearchTextFieldState
import org.jetbrains.jewel.ui.component.rememberSplitLayoutState
import org.jetbrains.jewel.ui.icon.IconKey
import org.jetbrains.jewel.ui.icons.AllIconsKeys
import org.jetbrains.jewel.ui.theme.colorPalette
import org.jetbrains.jewel.ui.theme.textFieldStyle
import org.jetbrains.jewel.ui.typography
import org.jetbrains.jewel.window.DecoratedWindow
import org.jetbrains.jewel.window.TitleBar
import org.jetbrains.jewel.window.newFullscreenControls
import java.awt.FileDialog
import java.io.File
import java.text.Collator
import java.util.Locale
import java.util.concurrent.atomic.AtomicInteger

private enum class Section(val title: () -> String, val icon: IconKey) {
    General({ Controller.trp("Settings", "General") }, AllIconsKeys.General.Settings),
    Discovery({ Controller.trp("Settings", "Discovery") }, AllIconsKeys.Actions.Find),
    Downloads({ Controller.trp("Settings", "Downloads") }, AllIconsKeys.Actions.Download),
    Extractor({ Controller.trp("Settings", "Extractor") }, AllIconsKeys.Nodes.Plugin),
    Postprocessing({ Controller.trp("Settings", "Postprocessing") }, AllIconsKeys.Actions.Compile),
}

private sealed class Setting(val label: String, val description: String?, val enabled: Boolean) {
    class Header(label: String, description: String? = null) : Setting(label, description, true)

    class Check(
        label: String,
        val checked: Boolean,
        val onChange: (Boolean) -> Unit,
        description: String? = null,
        enabled: Boolean = true,
    ) : Setting(label, description, enabled)

    class Combo(
        label: String,
        val items: List<String>,
        val selected: Int,
        val onChange: (Int) -> Unit,
        description: String? = null,
        enabled: Boolean = true,
    ) : Setting(label, description, enabled)

    class Numeric(
        label: String,
        val value: Int,
        val range: IntRange,
        val onChange: (Int) -> Unit,
        enabled: Boolean = true,
        description: String? = null,
    ) : Setting(label, description, enabled)

    class Field(
        label: String,
        val value: String,
        val onChange: (String) -> Unit,
        description: String? = null,
        enabled: Boolean = true,
    ) : Setting(label, description, enabled)

    class Custom(
        label: String,
        description: String? = null,
        val labeled: Boolean = false,
        val fill: Boolean = false,
        val content: @Composable (AnnotatedString) -> Unit = {},
    ) : Setting(label, description, true)
}

private fun labeled(label: String) = Controller.tr("{}:").replace("{}", label)

private fun Setting.matches(query: String) =
    label.contains(query, ignoreCase = true) || description?.contains(query, ignoreCase = true) == true

private fun highlight(text: String, query: String): AnnotatedString = buildAnnotatedString {
    append(text)
    if (query.isNotEmpty()) {
        var start = text.indexOf(query, ignoreCase = true)
        while (start >= 0) {
            addStyle(SpanStyle(background = Color(0x80FFC83D)), start, start + query.length)
            start = text.indexOf(query, start + query.length, ignoreCase = true)
        }
    }
}

@Composable
private fun SettingRow(setting: Setting, query: String, modifier: Modifier = Modifier) {
    val textColor = if (setting.enabled) Color.Unspecified else JewelTheme.globalColors.text.disabled
    Column(modifier, verticalArrangement = Arrangement.spacedBy(2.dp)) {
        when (setting) {
            is Setting.Header -> Row(
                Modifier.fillMaxWidth().padding(top = 8.dp),
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.spacedBy(8.dp),
            ) {
                Text(highlight(setting.label, query))
                Divider(Orientation.Horizontal, Modifier.weight(1f))
            }

            is Setting.Check -> Row(
                Modifier.clickable(enabled = setting.enabled) { setting.onChange(!setting.checked) },
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.spacedBy(4.dp),
            ) {
                Checkbox(setting.checked, setting.onChange, enabled = setting.enabled)
                Text(highlight(setting.label, query), color = textColor)
            }

            is Setting.Combo -> Row(
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.spacedBy(8.dp),
            ) {
                Text(highlight(labeled(setting.label), query), color = textColor)
                ListComboBox(
                    setting.items,
                    setting.selected,
                    setting.onChange,
                    Modifier.width(220.dp),
                    enabled = setting.enabled,
                )
            }

            is Setting.Numeric -> {
                val state = rememberTextFieldState(setting.value.toString())
                LaunchedEffect(state) {
                    snapshotFlow { state.text.toString() }.drop(1).collect { text ->
                        text.toIntOrNull()?.let { setting.onChange(it.coerceIn(setting.range)) }
                    }
                }
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(8.dp),
                ) {
                    Text(highlight(labeled(setting.label), query), color = textColor)
                    TextField(
                        state,
                        Modifier.width(100.dp).onFocusChanged {
                            if (!it.isFocused) {
                                val clamped = (state.text.toString().toIntOrNull() ?: setting.value)
                                    .coerceIn(setting.range)
                                    .toString()
                                state.edit { replace(0, length, clamped) }
                            }
                        },
                        enabled = setting.enabled,
                        inputTransformation = {
                            if (!asCharSequence().all(Char::isDigit) ||
                                length > setting.range.last.toString().length
                            ) {
                                revertAllChanges()
                            }
                        },
                    )
                }
            }

            is Setting.Field -> {
                val state = rememberTextFieldState(setting.value)
                LaunchedEffect(state) {
                    snapshotFlow { state.text.toString() }.drop(1).collect { setting.onChange(it) }
                }
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(8.dp),
                ) {
                    Text(highlight(labeled(setting.label), query), color = textColor)
                    TextField(state, Modifier.width(320.dp), enabled = setting.enabled)
                }
            }

            is Setting.Custom -> Box(if (setting.fill) Modifier.weight(1f).fillMaxWidth() else Modifier) {
                setting.content(highlight(if (setting.labeled) labeled(setting.label) else setting.label, query))
            }
        }
        setting.description?.let {
            InfoText(
                highlight(it, query),
                Modifier.padding(start = if (setting is Setting.Check) 28.dp else 0.dp),
                color = with(JewelTheme.globalColors.text) { if (setting.enabled) info else disabled },
                style = JewelTheme.typography.small,
            )
        }
    }
}

private fun select(
    selection: Set<Int>,
    anchor: Int,
    target: Int,
    keys: List<Int>,
    ctrl: Boolean,
    shift: Boolean,
): Set<Int> = when {
    ctrl -> if (target in selection) selection - target else selection + target

    shift && anchor in keys -> {
        val from = keys.indexOf(anchor)
        val to = keys.indexOf(target)
        keys.slice(minOf(from, to)..maxOf(from, to)).toSet()
    }

    else -> setOf(target)
}

private class ArgumentRow(val id: Int, text: String) {
    var text by mutableStateOf(text)
}

@Composable
private fun ArgumentsTable(label: AnnotatedString, value: String, onChange: (String) -> Unit) {
    val ids = remember { AtomicInteger() }
    val rows = remember {
        mutableStateListOf<ArgumentRow>().apply {
            val current = StringBuilder()
            var inQuotes = false
            var hasToken = false
            for (c in value) {
                when {
                    c == '"' -> {
                        inQuotes = !inQuotes
                        hasToken = true
                    }

                    !inQuotes && c.isWhitespace() -> if (hasToken) {
                        add(ArgumentRow(ids.getAndIncrement(), current.toString()))
                        current.clear()
                        hasToken = false
                    }

                    else -> {
                        current.append(c)
                        hasToken = true
                    }
                }
            }
            if (hasToken) {
                add(ArgumentRow(ids.getAndIncrement(), current.toString()))
            }
        }
    }
    var selection by remember { mutableStateOf(emptySet<Int>()) }
    var anchor by remember { mutableIntStateOf(-1) }
    val windowInfo = LocalWindowInfo.current
    val commit = {
        onChange(
            rows.map { it.text }
                .filter { it.isNotBlank() }
                .joinToString(" ") { if (it.any(Char::isWhitespace)) "\"$it\"" else it },
        )
    }
    Column(Modifier.fillMaxSize(), verticalArrangement = Arrangement.spacedBy(4.dp)) {
        if (label.isNotEmpty()) {
            Text(label)
        }
        Column(Modifier.fillMaxWidth().weight(1f).border(1.dp, JewelTheme.globalColors.borders.normal)) {
            Row(Modifier.fillMaxWidth().padding(4.dp), horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                IconButton(
                    {
                        val row = ArgumentRow(ids.getAndIncrement(), "")
                        rows.add(row)
                        selection = setOf(row.id)
                        anchor = row.id
                    },
                    Modifier.size(24.dp),
                ) { _ ->
                    Icon(AllIconsKeys.General.Add, Controller.tr("Add"), Modifier.size(16.dp))
                }
                IconButton(
                    {
                        rows.removeAll { it.id in selection }
                        selection = emptySet()
                        commit()
                    },
                    Modifier.size(24.dp),
                    enabled = rows.any { it.id in selection },
                ) { _ ->
                    Icon(AllIconsKeys.General.Remove, Controller.tr("Remove"), Modifier.size(16.dp))
                }
            }
            Divider(Orientation.Horizontal, Modifier.fillMaxWidth())
            Row(Modifier.fillMaxWidth().padding(horizontal = 8.dp, vertical = 4.dp)) {
                Text(Controller.tr("Argument"))
            }
            Divider(Orientation.Horizontal, Modifier.fillMaxWidth())
            Column(Modifier.fillMaxWidth().weight(1f).verticalScroll(rememberScrollState())) {
                rows.forEach { row ->
                    key(row.id) {
                        val state = rememberTextFieldState(row.text)
                        LaunchedEffect(state) {
                            snapshotFlow { state.text.toString() }.drop(1).collect { text ->
                                row.text = text
                                commit()
                            }
                        }
                        Row(
                            Modifier.fillMaxWidth()
                                .background(
                                    if (row.id in selection) {
                                        JewelTheme.colorPalette.rawMap.getValue("layer-1-bg")
                                    } else {
                                        Color.Transparent
                                    },
                                )
                                .clickable {
                                    val modifiers = windowInfo.keyboardModifiers
                                    selection = select(
                                        selection,
                                        anchor,
                                        row.id,
                                        rows.map { it.id },
                                        modifiers.isCtrlPressed || modifiers.isMetaPressed,
                                        modifiers.isShiftPressed,
                                    )
                                    if (!modifiers.isShiftPressed) {
                                        anchor = row.id
                                    }
                                }
                                .padding(horizontal = 8.dp, vertical = 2.dp),
                        ) {
                            TextField(
                                state,
                                Modifier.fillMaxWidth().onFocusChanged {
                                    if (it.hasFocus && row.id !in selection) {
                                        selection = setOf(row.id)
                                        anchor = row.id
                                    }
                                },
                            )
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun SettingsWindow(onClose: () -> Unit) {
    var configuration by remember { mutableStateOf(Controller.getConfiguration()) }
    var section by remember { mutableStateOf(Section.General) }
    var selectedArguments by remember { mutableStateOf(emptySet<Int>()) }
    var anchorArgument by remember { mutableIntStateOf(-1) }
    var tableVersion by remember { mutableIntStateOf(0) }
    val update = { transform: (Configuration) -> Configuration ->
        configuration = transform(configuration)
        Controller.setConfiguration(configuration)
    }
    val edit = { index: Int, change: (PostprocessingArgument) -> PostprocessingArgument ->
        update { current ->
            current.copy(
                postprocessingArguments = current.postprocessingArguments
                    .mapIndexed { position, item -> if (position == index) change(item) else item },
            )
        }
    }
    val displayLocale = when (configuration.translationLanguage) {
        "C" -> Locale.getDefault()
        else -> Locale.forLanguageTag(configuration.translationLanguage.replace('_', '-'))
    }
    val languages = remember(displayLocale) {
        Controller.getAvailableLanguages()
            .map { id ->
                id to Locale.forLanguageTag(id.replace('_', '-')).getDisplayName(displayLocale).ifBlank { id }
            }
            .sortedWith(compareBy(Collator.getInstance(displayLocale)) { it.second })
    }
    val languageIds = listOf("C") + languages.map { it.first }
    val historyLengths = listOf(0, 1, 7, 30, 90, 180, 365, -1)

    DecoratedWindow(
        onCloseRequest = onClose,
        state = rememberWindowState(size = DpSize(860.dp, 600.dp), position = WindowPosition(Alignment.Center)),
        title = Controller.tr("Settings"),
    ) {
        val pages: Map<Section, List<Setting>> = mapOf(
            Section.General to listOf(
                Setting.Header(Controller.tr("User Interface")),
                Setting.Combo(
                    Controller.tr("Theme"),
                    listOf(
                        Controller.trp("Theme", "Light"),
                        Controller.trp("Theme", "Dark"),
                        Controller.trp("Theme", "System"),
                    ),
                    configuration.theme,
                    { index ->
                        themeState.intValue = index
                        update { it.copy(theme = index) }
                    },
                ),
                Setting.Combo(
                    Controller.tr("Translation Language"),
                    listOf(Controller.trp("Language", "System")) + languages.map { it.second },
                    languageIds.indexOf(configuration.translationLanguage).coerceAtLeast(0),
                    { index ->
                        update { it.copy(translationLanguage = languageIds[index]) }
                        Controller.refreshLanguage()
                    },
                ),
                Setting.Header(Controller.tr("Updates")),
                Setting.Check(
                    Controller.tr("Automatically Check for Updates"),
                    configuration.automaticallyCheckForUpdates,
                    { checked -> update { it.copy(automaticallyCheckForUpdates = checked) } },
                ),
                Setting.Check(
                    Controller.tr("Allow Preview Updates"),
                    configuration.allowPreviewUpdates,
                    { checked -> update { it.copy(allowPreviewUpdates = checked) } },
                    Controller.tr("Update the app and dependencies, such as yt-dlp, to beta versions."),
                ),
                Setting.Header(Controller.tr("History")),
                Setting.Combo(
                    Controller.tr("Keep Download History For"),
                    listOf(
                        Controller.tr("Never"),
                        Controller.tr("1 Day"),
                        Controller.tr("1 Week"),
                        Controller.tr("1 Month"),
                        Controller.tr("3 Months"),
                        Controller.tr("6 Months"),
                        Controller.tr("1 Year"),
                        Controller.tr("Forever"),
                    ),
                    historyLengths.indexOf(configuration.historyLength).coerceAtLeast(0),
                    { index -> update { it.copy(historyLength = historyLengths[index]) } },
                ),
            ),
            Section.Downloads to listOf(
                Setting.Header(Controller.tr("Process")),
                Setting.Numeric(
                    Controller.tr("Maximum Active Downloads"),
                    configuration.maxNumberOfActiveDownloads,
                    1..10,
                    { value -> update { it.copy(maxNumberOfActiveDownloads = value) } },
                ),
                Setting.Check(
                    Controller.tr("Overwrite Existing Files"),
                    configuration.overwriteExistingFiles,
                    { checked -> update { it.copy(overwriteExistingFiles = checked) } },
                ),
                Setting.Check(
                    Controller.tr("Prevent Sleep During Downloads"),
                    configuration.preventSuspend,
                    { checked -> update { it.copy(preventSuspend = checked) } },
                ),
                Setting.Header(
                    Controller.tr("aria2"),
                    Controller.tr(
                        "An alternative downloader that may be faster in some regions compared to yt-dlp's native downloader.",
                    ),
                ),
                Setting.Check(
                    Controller.tr("Use aria2"),
                    configuration.useAria,
                    { checked -> update { it.copy(useAria = checked) } },
                ),
                Setting.Custom(Controller.tr("Learn More…")) { label ->
                    ExternalLink(label.text, "https://aria2.github.io")
                },
                Setting.Numeric(
                    Controller.tr("Maximum Connections Per Server"),
                    configuration.ariaMaxConnectionsPerServer,
                    1..16,
                    { value -> update { it.copy(ariaMaxConnectionsPerServer = value) } },
                    enabled = configuration.useAria,
                ),
                Setting.Numeric(
                    Controller.tr("Minimum Split Size (MiB)"),
                    configuration.ariaMinSplitSize,
                    1..1024,
                    { value -> update { it.copy(ariaMinSplitSize = value) } },
                    enabled = configuration.useAria,
                ),
                Setting.Header(
                    Controller.tr("Arguments"),
                    Controller.tr("These arguments will be appended to yt-dlp during the download phase."),
                ),
                Setting.Custom(
                    "",
                    fill = true,
                ) { label ->
                    ArgumentsTable(label, configuration.ytdlpDownloadArgs) { value ->
                        update { it.copy(ytdlpDownloadArgs = value) }
                    }
                },
            ),
            Section.Discovery to listOf(
                Setting.Header(Controller.tr("Process")),
                Setting.Check(
                    Controller.tr("Include Media Id in Title"),
                    configuration.includeMediaIdInTitle,
                    { checked -> update { it.copy(includeMediaIdInTitle = checked) } },
                    Controller.tr("Add the media entry's yt-dlp id to its default title."),
                ),
                Setting.Check(
                    Controller.tr("Limit Filename Characters"),
                    configuration.limitCharacters,
                    { checked -> update { it.copy(limitCharacters = checked) } },
                    Controller.tr("Restrict filename characters to those supported only by Windows."),
                ),
                Setting.Check(
                    Controller.tr("Show Super Resolution Formats"),
                    configuration.includeSuperResolutions,
                    { checked -> update { it.copy(includeSuperResolutions = checked) } },
                    Controller.tr(
                        "Super resolutions are resolutions that have been enhanced with artificial intelligence.",
                    ),
                ),
                Setting.Check(
                    Controller.tr("Show Auto-Generated Subtitles"),
                    configuration.includeAutoGeneratedSubtitles,
                    { checked -> update { it.copy(includeAutoGeneratedSubtitles = checked) } },
                ),
                Setting.Header(
                    Controller.tr("Preferences"),
                    Controller.tr("Prefer these codecs and options when showing formats for download."),
                ),
                Setting.Combo(
                    Controller.tr("Audio Codec"),
                    listOf(
                        Controller.tr("Any"),
                        Controller.tr("FLAC"),
                        Controller.tr("WAV"),
                        Controller.tr("Opus"),
                        Controller.tr("AAC"),
                        Controller.tr("MP4A"),
                        Controller.tr("MP3"),
                    ),
                    configuration.preferredAudioCodec,
                    { index -> update { it.copy(preferredAudioCodec = index) } },
                ),
                Setting.Combo(
                    Controller.tr("Video Codec"),
                    listOf(
                        Controller.tr("Any"),
                        Controller.tr("H.265"),
                        Controller.tr("VP9"),
                        Controller.tr("AV1"),
                        Controller.tr("H.264"),
                    ),
                    configuration.preferredVideoCodec,
                    { index -> update { it.copy(preferredVideoCodec = index) } },
                ),
                Setting.Combo(
                    Controller.tr("Subtitle Format"),
                    listOf(
                        Controller.tr("Any"),
                        Controller.tr("VTT"),
                        Controller.tr("SRT"),
                        Controller.tr("ASS"),
                        Controller.tr("LRC"),
                    ),
                    configuration.preferredSubtitleFormat,
                    { index -> update { it.copy(preferredSubtitleFormat = index) } },
                ),
                Setting.Combo(
                    Controller.tr("Frame Rate"),
                    listOf(
                        Controller.tr("Any"),
                        Controller.tr("24 FPS"),
                        Controller.tr("30 FPS"),
                        Controller.tr("60 FPS"),
                    ),
                    configuration.preferredFrameRate,
                    { index -> update { it.copy(preferredFrameRate = index) } },
                ),
                Setting.Header(
                    Controller.tr("Arguments"),
                    Controller.tr("These arguments will be appended to yt-dlp during the discovery phase."),
                ),
                Setting.Custom(
                    "",
                    fill = true,
                ) { label ->
                    ArgumentsTable(label, configuration.ytdlpDiscoveryArgs) { value ->
                        update { it.copy(ytdlpDiscoveryArgs = value) }
                    }
                },
            ),
            Section.Extractor to listOf(
                Setting.Header(Controller.tr("Process")),
                Setting.Check(
                    Controller.tr("Use Part Files"),
                    configuration.usePartFiles,
                    { checked -> update { it.copy(usePartFiles = checked) } },
                    Controller.tr("Download media in separate .part files instead of directly into the output file."),
                ),
                Setting.Check(
                    Controller.tr("Enable SponsorBlock for YouTube"),
                    configuration.youTubeSponsorBlock,
                    { checked -> update { it.copy(youTubeSponsorBlock = checked) } },
                    Controller.tr("Try to remove sponsored segments from videos."),
                ),
                Setting.Check(
                    Controller.tr("Translate Metadata and Chapters"),
                    configuration.translateMetadataAndChapters,
                    { checked -> update { it.copy(translateMetadataAndChapters = checked) } },
                    Controller.tr(
                        "Automatically translate embedded metadata and chapters using the app's set language on supported sites and media.",
                    ),
                ),
                Setting.Header(Controller.tr("Speed")),
                Setting.Check(
                    Controller.tr("Limit Download Speed"),
                    configuration.speedLimit != null,
                    { checked -> update { it.copy(speedLimit = if (checked) 1024 else null) } },
                ),
                Setting.Numeric(
                    Controller.tr("Speed Limit (KiB/s)"),
                    configuration.speedLimit ?: 1024,
                    512..10240,
                    { value -> update { it.copy(speedLimit = value) } },
                    enabled = configuration.speedLimit != null,
                ),
                Setting.Header(Controller.tr("Proxy")),
                Setting.Field(
                    Controller.tr("URL"),
                    configuration.proxyUrl,
                    { text -> update { it.copy(proxyUrl = text) } },
                ),
                Setting.Header(Controller.tr("Cookies")),
                Setting.Combo(
                    Controller.tr("Browser"),
                    listOf(
                        Controller.tr("None"),
                        Controller.tr("Brave"),
                        Controller.tr("Chrome"),
                        Controller.tr("Chromium"),
                        Controller.tr("Edge"),
                        Controller.tr("Firefox"),
                        Controller.tr("Opera"),
                        Controller.tr("Vivaldi"),
                        Controller.tr("Whale"),
                    ),
                    configuration.cookiesBrowser,
                    { index -> update { it.copy(cookiesBrowser = index) } },
                ),
                Setting.Custom(Controller.tr("File"), labeled = true) { label ->
                    Row(
                        Modifier.fillMaxWidth(),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(8.dp),
                    ) {
                        Text(label)
                        key(configuration.cookiesPath) {
                            TextField(
                                rememberTextFieldState(configuration.cookiesPath),
                                Modifier.weight(1f),
                                readOnly = true,
                            )
                        }
                        OutlinedButton({
                            val dialog = FileDialog(window, Controller.tr("Cookies File"), FileDialog.LOAD)
                            dialog.isVisible = true
                            val file = dialog.file
                            if (file != null) {
                                update { it.copy(cookiesPath = File(dialog.directory, file).path) }
                            }
                        }) {
                            Text(Controller.tr("Browse…"))
                        }
                        OutlinedButton(
                            { update { it.copy(cookiesPath = "") } },
                            enabled = configuration.cookiesPath.isNotEmpty(),
                        ) {
                            Text(Controller.tr("Clear"))
                        }
                    }
                },
            ),
            Section.Postprocessing to listOf(
                Setting.Header(Controller.tr("Metadata")),
                Setting.Check(
                    Controller.tr("Embed Metadata"),
                    configuration.embedMetadata,
                    { checked -> update { it.copy(embedMetadata = checked) } },
                ),
                Setting.Check(
                    Controller.tr("Remove Source Data"),
                    configuration.removeSourceData,
                    { checked -> update { it.copy(removeSourceData = checked) } },
                    Controller.tr("Clear metadata fields containing identifying download information."),
                    enabled = configuration.embedMetadata,
                ),
                Setting.Header(Controller.tr("Thumbnails")),
                Setting.Check(
                    Controller.tr("Embed Thumbnails"),
                    configuration.embedThumbnails,
                    { checked -> update { it.copy(embedThumbnails = checked) } },
                    Controller.tr(
                        "If the file type does not support embedding, the thumbnail will be written to a separate image file.",
                    ),
                ),
                Setting.Check(
                    Controller.tr("Crop Audio Thumbnails"),
                    configuration.cropAudioThumbnails,
                    { checked -> update { it.copy(cropAudioThumbnails = checked) } },
                    Controller.tr("Crop thumbnails of audio files to squares."),
                    enabled = configuration.embedThumbnails,
                ),
                Setting.Header(Controller.tr("Chapters")),
                Setting.Check(
                    Controller.tr("Embed Chapters"),
                    configuration.embedChapters,
                    { checked -> update { it.copy(embedChapters = checked) } },
                ),
                Setting.Header(Controller.tr("Subtitles")),
                Setting.Check(
                    Controller.tr("Embed Subtitles"),
                    configuration.embedSubtitles,
                    { checked -> update { it.copy(embedSubtitles = checked) } },
                    Controller.tr(
                        "If disabled or if embedding is not supported, downloaded subtitles will be saved to separate files.",
                    ),
                ),
                Setting.Header(Controller.tr("FFmpeg")),
                Setting.Numeric(
                    Controller.tr("Threads"),
                    configuration.postprocessingThreads,
                    1..Runtime.getRuntime().availableProcessors(),
                    { value -> update { it.copy(postprocessingThreads = value) } },
                    description = Controller.tr("Limit the number of threads used by ffmpeg."),
                ),
                Setting.Header(
                    Controller.tr("Arguments"),
                    Controller.tr("Arguments will be shown for selection in the download discovery dialog."),
                ),
                Setting.Custom(
                    "",
                    fill = true,
                ) {
                    val windowInfo = LocalWindowInfo.current
                    val steps = listOf(
                        Controller.tr("None"),
                        Controller.tr("Merger"),
                        Controller.tr("Modify Chapters"),
                        Controller.tr("Split Chapters"),
                        Controller.tr("Extract Audio"),
                        Controller.tr("Video Remuxer"),
                        Controller.tr("Video Converter"),
                        Controller.tr("Metadata"),
                        Controller.tr("Embed Subtitle"),
                        Controller.tr("Embed Thumbnail"),
                        Controller.tr("Subtitles Converter"),
                        Controller.tr("Thumbnails Converter"),
                        Controller.tr("Fixup Stretched"),
                        Controller.tr("Fixup M4a"),
                        Controller.tr("Fixup M3u8"),
                        Controller.tr("Fixup Timestamp"),
                        Controller.tr("Fixup Duration"),
                    )
                    val executables = listOf(
                        Controller.tr("None"),
                        Controller.tr("AtomicParsley"),
                        Controller.tr("FFmpeg"),
                        Controller.tr("FFprobe"),
                    )
                    Column(Modifier.fillMaxSize().border(1.dp, JewelTheme.globalColors.borders.normal)) {
                        Row(
                            Modifier.fillMaxWidth().padding(4.dp),
                            horizontalArrangement = Arrangement.spacedBy(4.dp),
                        ) {
                            IconButton(
                                {
                                    update { current ->
                                        current.copy(
                                            postprocessingArguments = current.postprocessingArguments +
                                                PostprocessingArgument(
                                                    generateSequence(1) { it + 1 }
                                                        .map { it.toString() }
                                                        .first { name ->
                                                            current.postprocessingArguments.none { it.name == name }
                                                        },
                                                ),
                                        )
                                    }
                                    selectedArguments = setOf(configuration.postprocessingArguments.size)
                                    anchorArgument = configuration.postprocessingArguments.size
                                    tableVersion++
                                },
                                Modifier.size(24.dp),
                            ) { _ ->
                                Icon(AllIconsKeys.General.Add, Controller.tr("Add"), Modifier.size(16.dp))
                            }
                            IconButton(
                                {
                                    update {
                                        it.copy(
                                            postprocessingArguments = it.postprocessingArguments
                                                .filterIndexed { index, _ -> index !in selectedArguments },
                                        )
                                    }
                                    selectedArguments = emptySet()
                                    tableVersion++
                                },
                                Modifier.size(24.dp),
                                enabled = selectedArguments.isNotEmpty(),
                            ) { _ ->
                                Icon(AllIconsKeys.General.Remove, Controller.tr("Remove"), Modifier.size(16.dp))
                            }
                        }
                        Divider(Orientation.Horizontal, Modifier.fillMaxWidth())
                        Row(
                            Modifier.fillMaxWidth().padding(horizontal = 8.dp, vertical = 4.dp),
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(Controller.tr("Name"), Modifier.weight(1.2f))
                            Text(Controller.tr("Post Processor"), Modifier.weight(1.4f))
                            Text(Controller.tr("Executable"), Modifier.weight(1f))
                            Text(Controller.tr("Argument"), Modifier.weight(2f))
                        }
                        Divider(Orientation.Horizontal, Modifier.fillMaxWidth())
                        Column(Modifier.fillMaxWidth().weight(1f).verticalScroll(rememberScrollState())) {
                            key(tableVersion) {
                                configuration.postprocessingArguments.forEachIndexed { index, argument ->
                                    key(index) {
                                        val nameState = rememberTextFieldState(argument.name)
                                        val argsState = rememberTextFieldState(argument.args)
                                        var nameTaken by remember { mutableStateOf(false) }
                                        val latest by rememberUpdatedState(configuration.postprocessingArguments)
                                        LaunchedEffect(nameState) {
                                            snapshotFlow { nameState.text.toString() }.drop(1).collect { text ->
                                                nameTaken = latest.withIndex()
                                                    .any { (position, item) ->
                                                        position != index &&
                                                            item.name == text
                                                    }
                                                if (!nameTaken && text.isNotBlank()) {
                                                    edit(index) { it.copy(name = text) }
                                                }
                                            }
                                        }
                                        LaunchedEffect(argsState) {
                                            snapshotFlow { argsState.text.toString() }.drop(1).collect { text ->
                                                edit(index) { it.copy(args = text) }
                                            }
                                        }
                                        Row(
                                            Modifier.fillMaxWidth()
                                                .background(
                                                    if (index in selectedArguments) {
                                                        JewelTheme.colorPalette.rawMap.getValue("layer-1-bg")
                                                    } else {
                                                        Color.Transparent
                                                    },
                                                )
                                                .onFocusChanged {
                                                    if (it.hasFocus && index !in selectedArguments) {
                                                        selectedArguments = setOf(index)
                                                        anchorArgument = index
                                                    }
                                                }
                                                .clickable {
                                                    val modifiers = windowInfo.keyboardModifiers
                                                    selectedArguments = select(
                                                        selectedArguments,
                                                        anchorArgument,
                                                        index,
                                                        configuration.postprocessingArguments.indices.toList(),
                                                        modifiers.isCtrlPressed || modifiers.isMetaPressed,
                                                        modifiers.isShiftPressed,
                                                    )
                                                    if (!modifiers.isShiftPressed) {
                                                        anchorArgument = index
                                                    }
                                                }
                                                .padding(horizontal = 8.dp, vertical = 2.dp),
                                            verticalAlignment = Alignment.CenterVertically,
                                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                                        ) {
                                            TextField(
                                                nameState,
                                                Modifier.weight(1.2f),
                                                outline = if (nameTaken) Outline.Error else Outline.None,
                                            )
                                            ListComboBox(
                                                steps,
                                                argument.step,
                                                { value -> edit(index) { it.copy(step = value) } },
                                                Modifier.weight(1.4f),
                                            )
                                            ListComboBox(
                                                executables,
                                                argument.executable,
                                                { value -> edit(index) { it.copy(executable = value) } },
                                                Modifier.weight(1f),
                                            )
                                            TextField(argsState, Modifier.weight(2f))
                                        }
                                    }
                                }
                            }
                        }
                    }
                },
            ),
        )
        val search = rememberSearchTextFieldState()
        val query = search.searchText.trim()
        val visible = Section.entries.filter { entry ->
            query.isEmpty() || entry.title().contains(query, ignoreCase = true) ||
                pages.getValue(entry).any { it.matches(query) }
        }
        val current = if (section in visible) section else visible.firstOrNull()

        MatchWindowBackground(window, JewelTheme.colorPalette.rawMap.getValue("dialog-bg"))
        TitleBar(Modifier.newFullscreenControls()) {}

        HorizontalSplitLayout(
            first = {
                Column(Modifier.fillMaxSize()) {
                    SearchTextField(
                        search,
                        Modifier.fillMaxWidth()
                            .padding(start = 8.dp, top = 8.dp, end = 8.dp, bottom = 6.dp)
                            .height(JewelTheme.textFieldStyle.metrics.minSize.height)
                            .background(
                                JewelTheme.colorPalette.rawMap.getValue("tool-window-bg"),
                                RoundedCornerShape(6.dp),
                            )
                            .border(
                                1.dp,
                                JewelTheme.colorPalette.rawMap.getValue("main-window-border"),
                                RoundedCornerShape(6.dp),
                            ),
                        placeholder = { Text(Controller.tr("Search")) },
                    )
                    VerticallyScrollableContainer(Modifier.fillMaxSize()) {
                        Column(Modifier.fillMaxWidth()) {
                            visible.forEach { entry ->
                                SimpleListItem(
                                    selected = current == entry,
                                    modifier = Modifier.fillMaxWidth().clickable { section = entry },
                                ) {
                                    Row(verticalAlignment = Alignment.CenterVertically) {
                                        Icon(entry.icon, null, Modifier.size(16.dp))
                                        Spacer(Modifier.width(6.dp))
                                        Text(highlight(entry.title(), query))
                                    }
                                }
                            }
                        }
                    }
                }
            },
            second = {
                BoxWithConstraints(Modifier.fillMaxSize()) {
                    VerticallyScrollableContainer(Modifier.fillMaxSize()) {
                        Column(
                            Modifier.fillMaxWidth().padding(16.dp),
                            verticalArrangement = Arrangement.spacedBy(10.dp),
                        ) {
                            if (current == null) {
                                Text(Controller.tr("Nothing found"))
                            } else {
                                Text(current.title(), fontWeight = FontWeight.Bold)
                                key(current) {
                                    pages.getValue(current).forEachIndexed { index, setting ->
                                        key(index) {
                                            SettingRow(
                                                setting,
                                                query,
                                                Modifier
                                                    .then(
                                                        if (setting is Setting.Custom && setting.fill) {
                                                            Modifier.height(
                                                                (this@BoxWithConstraints.maxHeight * 0.45f).coerceAtLeast(
                                                                    220.dp,
                                                                ),
                                                            )
                                                        } else {
                                                            Modifier
                                                        },
                                                    )
                                                    .padding(
                                                        start = if (
                                                            setting !is Setting.Header &&
                                                            pages.getValue(current).take(index).any {
                                                                it is Setting.Header
                                                            }
                                                        ) {
                                                            16.dp
                                                        } else {
                                                            0.dp
                                                        },
                                                    ),
                                            )
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            },
            modifier = Modifier.fillMaxSize().background(JewelTheme.colorPalette.rawMap.getValue("dialog-bg")),
            firstPaneMinWidth = 170.dp,
            secondPaneMinWidth = 420.dp,
            state = rememberSplitLayoutState(0.26f),
        )
    }
}
