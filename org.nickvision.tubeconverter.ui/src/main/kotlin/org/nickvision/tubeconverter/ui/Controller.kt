package org.nickvision.tubeconverter.ui

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
    private val freeStringHandle = bind("controller_free_string", FunctionDescriptor.ofVoid(ADDRESS))
    private val initHandle = bind("controller_init", FunctionDescriptor.ofVoid(ADDRESS))
    private val trHandle = bind("controller_tr", FunctionDescriptor.of(ADDRESS, ADDRESS))
    private val trnHandle = bind("controller_trn", FunctionDescriptor.of(ADDRESS, ADDRESS, ADDRESS, JAVA_LONG))
    private val trpHandle = bind("controller_trp", FunctionDescriptor.of(ADDRESS, ADDRESS, ADDRESS))
    private val trpnHandle = bind(
        "controller_trpn",
        FunctionDescriptor.of(ADDRESS, ADDRESS, ADDRESS, ADDRESS, JAVA_LONG),
    )

    fun init(localeDir: String) {
        Arena.ofConfined().use { arena -> initHandle.invoke(arena.allocateFrom(localeDir)) }
    }

    fun tr(msgid: String): String = Arena.ofConfined().use { arena ->
        take(trHandle.invoke(arena.allocateFrom(msgid)) as MemorySegment, msgid)
    }

    fun trn(msgid: String, msgidPlural: String, n: Long): String = Arena.ofConfined().use { arena ->
        take(
            trnHandle.invoke(arena.allocateFrom(msgid), arena.allocateFrom(msgidPlural), n) as MemorySegment,
            if (n == 1L) msgid else msgidPlural,
        )
    }

    fun trp(context: String, msgid: String): String = Arena.ofConfined().use { arena ->
        take(trpHandle.invoke(arena.allocateFrom(context), arena.allocateFrom(msgid)) as MemorySegment, msgid)
    }

    fun trpn(context: String, msgid: String, msgidPlural: String, n: Long): String = Arena.ofConfined().use { arena ->
        take(
            trpnHandle.invoke(
                arena.allocateFrom(context),
                arena.allocateFrom(msgid),
                arena.allocateFrom(msgidPlural),
                n,
            ) as MemorySegment,
            if (n == 1L) msgid else msgidPlural,
        )
    }

    private fun bind(name: String, descriptor: FunctionDescriptor): MethodHandle =
        linker.downcallHandle(library.find(name).orElseThrow { UnsatisfiedLinkError(name) }, descriptor)

    private fun take(pointer: MemorySegment, fallback: String): String {
        if (pointer == MemorySegment.NULL) {
            return fallback
        }
        try {
            return pointer.reinterpret(Long.MAX_VALUE).getString(0)
        } finally {
            freeStringHandle.invoke(pointer)
        }
    }
}
