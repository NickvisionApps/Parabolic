import org.jetbrains.compose.desktop.application.dsl.TargetFormat

plugins {
    kotlin("jvm") version "2.3.21"
    id("org.jetbrains.kotlin.plugin.compose") version "2.3.21"
    kotlin("plugin.serialization") version "2.3.21"
    id("org.jetbrains.compose") version "1.12.1"
    id("com.diffplug.spotless") version "8.11.0"
}

val ijp = "263.7307.200"
val jewel = "0.42.0-$ijp"

providers.gradleProperty("uiBuildDir").orNull?.let { layout.buildDirectory = file(it) }

kotlin {
    jvmToolchain {
        languageVersion = JavaLanguageVersion.of(25)
        vendor = JvmVendorSpec.JETBRAINS
    }
}

tasks.matching { it.name == "createDistributable" }.configureEach {
    inputs.files(fileTree(layout.buildDirectory.dir("app-resources")))
        .withPropertyName("nativeAppResources")
        .withPathSensitivity(PathSensitivity.RELATIVE)
}

spotless {
    kotlin {
        target("src/**/*.kt")
        ktlint("1.8.0")
    }
    kotlinGradle {
        target("*.kts")
        ktlint("1.8.0")
    }
}

dependencies {
    implementation(compose.desktop.currentOs) {
        exclude(group = "org.jetbrains.compose.material")
    }
    implementation("org.jetbrains.jewel:jewel-int-ui-standalone:$jewel")
    implementation("org.jetbrains.jewel:jewel-int-ui-decorated-window:$jewel")
    implementation("com.jetbrains.intellij.platform:icons:$ijp")
    implementation("org.jetbrains.kotlinx:kotlinx-serialization-json:1.13.0")
}

compose.desktop {
    application {
        mainClass = "org.nickvision.tubeconverter.ui.MainKt"
        javaHome = javaToolchains.launcherFor(java.toolchain).get()
            .metadata.installationPath.asFile.absolutePath
        jvmArgs += listOf(
            "--enable-native-access=ALL-UNNAMED",
            "-Dskiko.rendering.macos.waitForPreviousFrameVsyncOnRedrawImmediately=false",
        )

        nativeDistributions {
            appResourcesRootDir = layout.buildDirectory.dir("app-resources")
            includeAllModules = true
            targetFormats(TargetFormat.Dmg, TargetFormat.Msi, TargetFormat.Deb)
            packageName = "Parabolic"
            packageVersion = "1.0.0"
        }
    }
}
