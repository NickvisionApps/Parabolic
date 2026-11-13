package org.nickvision.tubeconverter.ui

import androidx.compose.runtime.mutableIntStateOf
import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import java.io.File
import java.lang.foreign.Arena
import java.lang.foreign.FunctionDescriptor
import java.lang.foreign.Linker
import java.lang.foreign.MemorySegment
import java.lang.foreign.SymbolLookup
import java.lang.foreign.ValueLayout.ADDRESS
import java.lang.foreign.ValueLayout.JAVA_LONG
import java.lang.invoke.MethodHandle

object Controller {
    private val linker = Linker.nativeLinker()
    private val library = SymbolLookup.libraryLookup(
        File(
            checkNotNull(System.getProperty("compose.application.resources.dir")) {
                "Run via Gradle or the packaged app"
            },
            System.mapLibraryName("parabolic"),
        ).toPath(),
        Arena.global(),
    )
    private val languageVersion = mutableIntStateOf(0)
    private val json = Json {
        encodeDefaults = true
        ignoreUnknownKeys = true
    }
    private val freeStringHandle = bind("controller_free_string", FunctionDescriptor.ofVoid(ADDRESS))
    private val getAvailableLanguagesHandle = bind("controller_get_available_languages", FunctionDescriptor.of(ADDRESS))
    private val getConfigurationHandle = bind("controller_get_configuration", FunctionDescriptor.of(ADDRESS))
    private val getDiscussionsUrlHandle = bind("controller_get_discussions_url", FunctionDescriptor.of(ADDRESS))
    private val getIssuesUrlHandle = bind("controller_get_issues_url", FunctionDescriptor.of(ADDRESS))
    private val getRepoUrlHandle = bind("controller_get_repo_url", FunctionDescriptor.of(ADDRESS))
    private val initHandle = bind("controller_init", FunctionDescriptor.ofVoid(ADDRESS))
    private val setConfigurationHandle = bind("controller_set_configuration", FunctionDescriptor.ofVoid(ADDRESS))
    private val trHandle = bind("controller_tr", FunctionDescriptor.of(ADDRESS, ADDRESS))
    private val trnHandle = bind("controller_trn", FunctionDescriptor.of(ADDRESS, ADDRESS, ADDRESS, JAVA_LONG))
    private val trpHandle = bind("controller_trp", FunctionDescriptor.of(ADDRESS, ADDRESS, ADDRESS))
    private val trpnHandle = bind(
        "controller_trpn",
        FunctionDescriptor.of(ADDRESS, ADDRESS, ADDRESS, ADDRESS, JAVA_LONG),
    )

    fun getAvailableLanguages(): List<String> = json.decodeFromString(
        checkNotNull(take(getAvailableLanguagesHandle.invoke() as MemorySegment)) {
            "Controller is not initialized"
        },
    )

    fun getConfiguration(): Configuration = json.decodeFromString(
        checkNotNull(take(getConfigurationHandle.invoke() as MemorySegment)) {
            "Controller is not initialized"
        },
    )

    fun getDiscussionsUrl(): String = checkNotNull(take(getDiscussionsUrlHandle.invoke() as MemorySegment)) {
        "Controller is not initialized"
    }

    fun getIssuesUrl(): String = checkNotNull(take(getIssuesUrlHandle.invoke() as MemorySegment)) {
        "Controller is not initialized"
    }

    fun getRepoUrl(): String = checkNotNull(take(getRepoUrlHandle.invoke() as MemorySegment)) {
        "Controller is not initialized"
    }

    fun init(resourceDir: String) {
        Arena.ofConfined().use { arena -> initHandle.invoke(arena.allocateFrom(resourceDir)) }
    }

    fun refreshLanguage() {
        languageVersion.intValue++
    }

    fun setConfiguration(configuration: Configuration) {
        Arena.ofConfined().use { arena ->
            setConfigurationHandle.invoke(arena.allocateFrom(json.encodeToString(configuration)))
        }
    }

    fun tr(msgid: String): String = Arena.ofConfined().use { arena ->
        track()
        take(trHandle.invoke(arena.allocateFrom(msgid)) as MemorySegment) ?: msgid
    }

    fun trn(msgid: String, msgidPlural: String, n: Long): String = Arena.ofConfined().use { arena ->
        track()
        take(trnHandle.invoke(arena.allocateFrom(msgid), arena.allocateFrom(msgidPlural), n) as MemorySegment)
            ?: if (n == 1L) msgid else msgidPlural
    }

    fun trp(context: String, msgid: String): String = Arena.ofConfined().use { arena ->
        track()
        take(trpHandle.invoke(arena.allocateFrom(context), arena.allocateFrom(msgid)) as MemorySegment) ?: msgid
    }

    fun trpn(context: String, msgid: String, msgidPlural: String, n: Long): String = Arena.ofConfined().use { arena ->
        track()
        take(
            trpnHandle.invoke(
                arena.allocateFrom(context),
                arena.allocateFrom(msgid),
                arena.allocateFrom(msgidPlural),
                n,
            ) as MemorySegment,
        ) ?: if (n == 1L) msgid else msgidPlural
    }

    private fun track() = languageVersion.intValue

    private fun bind(name: String, descriptor: FunctionDescriptor): MethodHandle =
        linker.downcallHandle(library.find(name).orElseThrow { UnsatisfiedLinkError(name) }, descriptor)

    private fun take(pointer: MemorySegment): String? {
        if (pointer == MemorySegment.NULL) {
            return null
        }
        try {
            return pointer.reinterpret(Long.MAX_VALUE).getString(0)
        } finally {
            freeStringHandle.invoke(pointer)
        }
    }
}

@Serializable
data class Configuration(
    val allowPreviewUpdates: Boolean = false,
    val ariaMaxConnectionsPerServer: Int = 16,
    val ariaMinSplitSize: Int = 20,
    val audioBitrate: Double = Double.MAX_VALUE,
    val audioFormatIds: Map<String, String> = emptyMap(),
    val audioOnlyFileType: Int = 7,
    val automaticallyCheckForUpdates: Boolean = true,
    val cookiesBrowser: Int = 0,
    val cookiesPath: String = "",
    val cropAudioThumbnails: Boolean = false,
    val downloadImmediatelyAsAudio: Boolean = false,
    val downloadImmediatelyAsVideo: Boolean = false,
    val embedChapters: Boolean = false,
    val embedMetadata: Boolean = true,
    val embedSubtitles: Boolean = true,
    val embedThumbnails: Boolean = true,
    val exportDescription: Boolean = false,
    val exportM3U: Boolean = false,
    val fullFileType: Int = 1,
    val historyLength: Int = 7,
    val historySortNewestFirst: Boolean = true,
    val includeAutoGeneratedSubtitles: Boolean = true,
    val includeMediaIdInTitle: Boolean = true,
    val includeSuperResolutions: Boolean = true,
    val limitCharacters: Boolean = System.getProperty("os.name").startsWith("Windows"),
    val maxNumberOfActiveDownloads: Int = 5,
    val numberTitles: Boolean = false,
    val overwriteExistingFiles: Boolean = true,
    val postprocessingArgumentNames: List<String> = emptyList(),
    val postprocessingArguments: List<PostprocessingArgument> = emptyList(),
    val postprocessingThreads: Int = Runtime.getRuntime().availableProcessors(),
    val preferredAudioCodec: Int = 0,
    val preferredFrameRate: Int = 0,
    val preferredSubtitleFormat: Int = 0,
    val preferredVideoCodec: Int = 0,
    val preventSuspend: Boolean = false,
    val proxyUrl: String = "",
    val removeSourceData: Boolean = false,
    val reverseDownloadOrder: Boolean = false,
    val saveFolder: String = "",
    val showDisclaimerOnStartup: Boolean = true,
    val speedLimit: Int? = null,
    val splitChapters: Boolean = false,
    val subtitleLanguages: List<SubtitleLanguage> = emptyList(),
    val theme: Int = 2,
    val translateMetadataAndChapters: Boolean = true,
    val translationLanguage: String = "C",
    val useAria: Boolean = false,
    val usePartFiles: Boolean = true,
    val videoFormatIds: Map<String, String> = emptyMap(),
    val videoOnlyFileType: Int = 1,
    val videoResolution: Resolution = Resolution(Int.MAX_VALUE, Int.MAX_VALUE),
    val windowGeometry: WindowGeometry = WindowGeometry(),
    val youTubeSponsorBlock: Boolean = false,
    val ytdlpDiscoveryArgs: String = "",
    val ytdlpDownloadArgs: String = "",
)

@Serializable
data class PostprocessingArgument(
    val name: String = "",
    val step: Int = 0,
    val executable: Int = 0,
    val args: String = "",
)

@Serializable
data class Resolution(val width: Int, val height: Int)

@Serializable
data class SubtitleLanguage(val language: String = "", val isAutoGenerated: Boolean = false)

@Serializable
data class WindowGeometry(
    val height: Int = 600,
    val isMaximized: Boolean = false,
    val width: Int = 800,
    val x: Int = 10,
    val y: Int = 10,
)
