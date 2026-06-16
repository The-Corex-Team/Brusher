#pragma once

class QApplication;

/**
 * @brief Cross-platform theme manager for Brusher.
 *
 * Forces the Qt Fusion style, applies a comprehensive dark QPalette,
 * and loads the exhaustive QSS stylesheet from the Qt resource system.
 * This ensures identical appearance on Linux, Windows, and macOS.
 */
class BrusherTheme {
public:
    /**
     * @brief Apply the Brusher dark theme to the application.
     * 
     * Must be called after QApplication construction but before
     * any widgets are created. Sets the Fusion style, configures
     * the global QPalette, and loads the QSS stylesheet.
     */
    static void apply(QApplication &app);

private:
    BrusherTheme() = delete; // Static-only class
};
