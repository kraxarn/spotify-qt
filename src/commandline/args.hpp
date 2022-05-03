#pragma once

/**
 * Enable developer mode
 */
#define ARG_ENABLE_DEV QStringLiteral("dev")

/**
 * Force show setup dialog on start
 */
#define ARG_FORCE_SETUP QStringLiteral("reset-credentials")

/**
 * List paths and close application
 */
#define ARG_LIST_PATHS QStringLiteral("paths")

/**
 * Don't start IPC server on app start
 * @note Not implemented
 */
#define ARG_DISABLE_IPC QStringLiteral("multiple-instances")

/**
 * Play/pause currently playing audio
 */
#define ARG_PLAY_PAUSE QStringLiteral("play-pause")

/**
 * Go to previous track
 * @note Not implemented
 */
#define ARG_PREVIOUS_TRACK QStringLiteral("previous")

/**
 * Go to the next track
 * @note Not implemented
 */
#define ARG_NEXT_TRACK QStringLiteral("next")
