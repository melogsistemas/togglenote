#pragma once
#include <QString>

namespace Constants
{
inline const auto APP_NAME    = QStringLiteral("togglenote");
inline const auto APP_VERSION = QStringLiteral("1.0.0");

namespace Paths
{
inline const auto THEME = QStringLiteral(":/theme.qss");
} // namespace Paths

namespace IPC
{
#ifdef TN_DEBUG
inline const auto SERVER = QStringLiteral("togglenote-ipc-debug");
#else
inline const auto SERVER = QStringLiteral("togglenote-ipc");
#endif
} // namespace IPC
} // namespace Constants
