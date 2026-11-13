pluginManagement {
    repositories {
        gradlePluginPortal()
        mavenCentral()
        google()
    }
}

plugins {
    id("org.gradle.toolchains.foojay-resolver-convention") version "1.0.0"
}

dependencyResolutionManagement {
    repositories {
        mavenCentral()
        google()
        maven("https://www.jetbrains.com/intellij-repository/releases") {
            content { includeModule("com.jetbrains.intellij.platform", "icons") }
        }
    }
}

rootProject.name = "parabolic"
