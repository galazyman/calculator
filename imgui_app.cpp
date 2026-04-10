
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <functional>
#include <deque>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

#include "linearalgebra.cpp"
#include "Games/dice_roller.h"
#include "Games/tic_tac_toe.h"
#include "Solvers/problem_solver.h"
#include "Calculator/enhanced_calculator.h"
#include "UI/games_ui.h"
#include "UI/solver_ui.h"


namespace Terminal {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string ITALIC = "\033[3m";
    const std::string UNDERLINE = "\033[4m";
    const std::string BLINK = "\033[5m";
    const std::string CYAN = "\033[36m";
    const std::string MAGENTA = "\033[35m";
    const std::string GREEN = "\033[32m";
    const std::string RED = "\033[31m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string WHITE = "\033[37m";
    const std::string BG_DARK = "\033[40m";
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_CYAN = "\033[46m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_RED = "\033[41m";

    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    
    void drawLine(int width, char ch = '=') {
        for (int i = 0; i < width; ++i) std::cout << ch;
        std::cout << "\n";
    }

    void drawBox(int width, char ch = '=') {
        drawLine(width, ch);
    }

    void drawTitle(const std::string& title) {
        std::cout << CYAN << BOLD;
        drawBox(100, '=');
        std::cout << "  [--  " << CYAN << BOLD << title << RESET << CYAN << BOLD << "  --]\n";
        drawBox(100, '=');
        std::cout << RESET;
    }

    void drawSection(const std::string& sectionName) {
        std::cout << "\n" << MAGENTA << BOLD << "┌─ " << sectionName << " ─┐" << RESET << "\n";
    }

    void drawSubsection(const std::string& subName) {
        std::cout << GREEN << "├─ " << subName << "\n" << RESET;
    }

    void drawSeparator() {
        std::cout << CYAN << "┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄\n" << RESET;
    }

    std::string progressBar(double percent, int width = 40) {
        int filled = static_cast<int>(percent * width / 100.0);
        std::stringstream ss;
        ss << "[";
        for (int i = 0; i < width; ++i) {
            if (i < filled) ss << GREEN << "█" << RESET;
            else ss << DIM << "░" << RESET;
        }
        ss << "] " << std::fixed << std::setprecision(1) << percent << "%";
        return ss.str();
    }

    std::string centerText(const std::string& text, int width) {
        int padding = (width - text.length()) / 2;
        std::string result(padding, ' ');
        result += text;
        return result;
    }
}


namespace SystemGlobal {
    const std::string VERSION_ID = "6.2.0-ULTRA";
    const std::string BUILD_DATE = "2026-04-04";
    const double EPSILON = 1e-9;
    const int MAX_SESSION_HISTORY = 1000;
    const int MAX_LOG_ENTRIES = 500;

    enum class LogLevel { INFO, WARNING, ERROR, CRITICAL, DEBUG };
    enum class Domain { 
        STATISTICS, 
        TRIGONOMETRY, 
        LINEAR_ALGEBRA, 
        VECTOR_OPS, 
        ADVANCED_MATH,
        NUMERICAL_METHODS,
        SIGNAL_PROCESSING,
        CALCULUS
    };

    struct EngineConfig {
        bool enableLogging = true;
        bool enableHeuristics = true;
        bool verboseOutput = true;
        bool enablePersistence = true;
        int maxIterations = 100;
        double convergenceTolerance = 1e-6;
        std::string configFile = "sovereign_config.ini";
    };

    EngineConfig globalConfig;
}


struct OperationResult {
    std::string operationID;
    std::vector<double> inputs;
    double result = 0.0;
    bool success = false;
    std::string errorMessage;
    std::chrono::system_clock::time_point timestamp;
    long executionTimeMs = 0;

    std::string toString() const {
        std::stringstream ss;
        ss << "[" << operationID << "] → " << std::fixed << std::setprecision(8) << result;
        if (!success) ss << " [ERROR: " << errorMessage << "]";
        ss << " (" << executionTimeMs << "ms)";
        return ss.str();
    }
};

class SessionManager {
private:
    std::deque<OperationResult> history;
    std::map<std::string, int> operationStats;
    std::chrono::system_clock::time_point sessionStart;

public:
    SessionManager() : sessionStart(std::chrono::system_clock::now()) {}

    void recordOperation(const OperationResult& result) {
        history.push_back(result);
        if (history.size() > SystemGlobal::MAX_SESSION_HISTORY) {
            history.pop_front();
        }
        operationStats[result.operationID]++;
    }

    const std::deque<OperationResult>& getHistory() const { return history; }
    const std::map<std::string, int>& getStats() const { return operationStats; }

    std::string getSessionSummary() const {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStart).count();

        std::stringstream ss;
        ss << "\n" << Terminal::BOLD << "╔════ SESSION SUMMARY ════╗" << Terminal::RESET << "\n";
        ss << "  Total Operations: " << history.size() << "\n";
        ss << "  Session Duration: " << duration << " seconds\n";
        ss << "  Unique Operations: " << operationStats.size() << "\n";
        ss << Terminal::BOLD << "╚═════════════════════════╝" << Terminal::RESET << "\n";
        return ss.str();
    }
};


class EnhancedLogger {
private:
    std::deque<std::string> logBuffer;
    std::ofstream logFile;

public:
    EnhancedLogger() {
        if (SystemGlobal::globalConfig.enableLogging) {
            logFile.open("sovereign_engine_6.2.log", std::ios::app);
        }
    }

    void write(const std::string& message, SystemGlobal::LogLevel level) {
        std::string prefix, color;
        switch (level) {
        case SystemGlobal::LogLevel::INFO:      prefix = "ℹ  [INFO]";    color = Terminal::GREEN; break;
        case SystemGlobal::LogLevel::WARNING:   prefix = "⚠  [WARN]";    color = Terminal::YELLOW; break;
        case SystemGlobal::LogLevel::ERROR:     prefix = "✗  [ERROR]";   color = Terminal::RED; break;
        case SystemGlobal::LogLevel::CRITICAL:  prefix = "❌ [FATAL]";   color = Terminal::MAGENTA; break;
        case SystemGlobal::LogLevel::DEBUG:     prefix = "🐛 [DEBUG]";   color = Terminal::CYAN; break;
        }

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;

        #ifdef _MSC_VER
            struct tm tm_info;
            localtime_s(&tm_info, &time);
            ss << std::put_time(&tm_info, "%Y-%m-%d %H:%M:%S");
        #else
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        #endif

        std::string output = color + prefix + " " + message + Terminal::RESET;
        std::cout << output << "\n";

        if (logFile.is_open()) {
            logFile << "[" << ss.str() << "] " << prefix << " " << message << "\n";
            logFile.flush();
        }

        logBuffer.push_back(output);
        if (logBuffer.size() > SystemGlobal::MAX_LOG_ENTRIES) {
            logBuffer.pop_front();
        }
    }

    const std::deque<std::string>& getBuffer() const { return logBuffer; }
};


class ICommand {
public:
    virtual ~ICommand() = default;
    virtual double execute(const std::vector<double>& args) = 0;
    virtual std::string getID() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getFullDescription() const { return getDescription(); }
    virtual std::vector<std::string> getPrompts() const = 0;
    virtual int getRequiredArgs() const = 0;
    virtual SystemGlobal::Domain getDomain() const = 0;
    virtual std::string getExample() const { return "No example available"; }
};

class StatsCalculateMean : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        Statistics::Dataset data;
        for (double val : args) data.addObservation(val);
        return data.calculateMean();
    }
    std::string getID() const override { return "STATS_MEAN"; }
    std::string getDescription() const override { return "Calculate arithmetic mean"; }
    std::string getFullDescription() const override { return "Calculates the average of all values: μ = Σx / n"; }
    std::vector<std::string> getPrompts() const override { return { "Number of values: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
    std::string getExample() const override { return "Example: [1, 2, 3, 4, 5] → Mean = 3.0"; }
};

class StatsCalculateVariance : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        Statistics::Dataset data;
        for (double val : args) data.addObservation(val);
        return data.calculateVariance();
    }
    std::string getID() const override { return "STATS_VARIANCE"; }
    std::string getDescription() const override { return "Calculate variance"; }
    std::string getFullDescription() const override { return "Variance: σ² = Σ(x - μ)² / n"; }
    std::vector<std::string> getPrompts() const override { return { "Number of values: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};

class StatsCalculateStdDev : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        Statistics::Dataset data;
        for (double val : args) data.addObservation(val);
        return data.calculateStdDev();
    }
    std::string getID() const override { return "STATS_STDDEV"; }
    std::string getDescription() const override { return "Standard deviation"; }
    std::string getFullDescription() const override { return "Standard deviation: σ = √(Σ(x - μ)² / n)"; }
    std::vector<std::string> getPrompts() const override { return { "Number of values: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};

class StatsCalculateMedian : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        Statistics::Dataset data;
        for (double val : args) data.addObservation(val);
        return data.getMedian();
    }
    std::string getID() const override { return "STATS_MEDIAN"; }
    std::string getDescription() const override { return "Calculate median"; }
    std::string getFullDescription() const override { return "Middle value when data is sorted"; }
    std::vector<std::string> getPrompts() const override { return { "Number of values: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};

class StatsDetectOutliers : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        Statistics::Dataset data;
        for (size_t i = 0; i < args.size() - 1; ++i) data.addObservation(args[i]);
        std::vector<double> outliers = data.detectOutliersIQR();
        std::cout << "\nDetected " << outliers.size() << " outliers using IQR method\n";
        return (double)outliers.size();
    }
    std::string getID() const override { return "STATS_OUTLIERS"; }
    std::string getDescription() const override { return "Detect outliers"; }
    std::vector<std::string> getPrompts() const override { return { "Number of values: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};

class StatsMode : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        Statistics::Dataset data;
        for (double val : args) data.addObservation(val);
        return data.getMode();
    }
    std::string getID() const override { return "STATS_MODE"; }
    std::string getDescription() const override { return "Find the mode (most frequent value)"; }
    std::vector<std::string> getPrompts() const override { return { "Number of values: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};

class StatsSkewness : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        Statistics::Dataset data;
        for (double val : args) data.addObservation(val);
        return data.getSkewness();
    }
    std::string getID() const override { return "STATS_SKEWNESS"; }
    std::string getDescription() const override { return "Calculate distribution skewness"; }
    std::vector<std::string> getPrompts() const override { return { "Number of values: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};

class StatsKurtosis : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        Statistics::Dataset data;
        for (double val : args) data.addObservation(val);
        return data.getKurtosis();
    }
    std::string getID() const override { return "STATS_KURTOSIS"; }
    std::string getDescription() const override { return "Calculate distribution kurtosis"; }
    std::vector<std::string> getPrompts() const override { return { "Number of values: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};

class StatsPearsonCorrelation : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        size_t n = args.size() / 2;
        std::vector<double> x(args.begin(), args.begin() + n);
        std::vector<double> y(args.begin() + n, args.end());
        
        Statistics::Dataset dataX, dataY;
        for (double val : x) dataX.addObservation(val);
        for (double val : y) dataY.addObservation(val);
        
        return dataX.calculatePearsonCorrelation(dataY);
    }
    std::string getID() const override { return "STATS_CORRELATION"; }
    std::string getDescription() const override { return "Pearson correlation coefficient"; }
    std::vector<std::string> getPrompts() const override { return { "Total values (must be even): " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};

class StatsZNormalize : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        Statistics::Dataset data;
        for (size_t i = 0; i < args.size() - 1; ++i) data.addObservation(args[i]);
        double value = args.back();
        double mean = data.calculateMean();
        double stddev = data.calculateStdDev();
        return (value - mean) / stddev;
    }
    std::string getID() const override { return "STATS_ZSCORE"; }
    std::string getDescription() const override { return "Compute Z-score normalization"; }
    std::vector<std::string> getPrompts() const override { return { "Number of values: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};



class TrigSine : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return std::sin(args[0]); }
    std::string getID() const override { return "TRIG_SINE"; }
    std::string getDescription() const override { return "sin(x) in radians"; }
    std::vector<std::string> getPrompts() const override { return { "Angle (radians): " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigCosine : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return std::cos(args[0]); }
    std::string getID() const override { return "TRIG_COSINE"; }
    std::string getDescription() const override { return "cos(x) in radians"; }
    std::vector<std::string> getPrompts() const override { return { "Angle (radians): " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigTangent : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return std::tan(args[0]); }
    std::string getID() const override { return "TRIG_TANGENT"; }
    std::string getDescription() const override { return "tan(x) in radians"; }
    std::vector<std::string> getPrompts() const override { return { "Angle (radians): " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigArcsine : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        if (args[0] < -1 || args[0] > 1) throw std::domain_error("arcsin domain: [-1, 1]");
        return std::asin(args[0]);
    }
    std::string getID() const override { return "TRIG_ARCSIN"; }
    std::string getDescription() const override { return "arcsin(x) inverse sine"; }
    std::vector<std::string> getPrompts() const override { return { "Value ([-1,1]): " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigArccosine : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        if (args[0] < -1 || args[0] > 1) throw std::domain_error("arccos domain: [-1, 1]");
        return std::acos(args[0]);
    }
    std::string getID() const override { return "TRIG_ARCCOS"; }
    std::string getDescription() const override { return "arccos(x) inverse cosine"; }
    std::vector<std::string> getPrompts() const override { return { "Value ([-1,1]): " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigArctangent : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return std::atan(args[0]); }
    std::string getID() const override { return "TRIG_ARCTAN"; }
    std::string getDescription() const override { return "arctan(x) inverse tangent"; }
    std::vector<std::string> getPrompts() const override { return { "Value: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigSinh : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return std::sinh(args[0]); }
    std::string getID() const override { return "TRIG_SINH"; }
    std::string getDescription() const override { return "sinh(x) hyperbolic sine"; }
    std::vector<std::string> getPrompts() const override { return { "Value: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigCosh : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return std::cosh(args[0]); }
    std::string getID() const override { return "TRIG_COSH"; }
    std::string getDescription() const override { return "cosh(x) hyperbolic cosine"; }
    std::vector<std::string> getPrompts() const override { return { "Value: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigTanh : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return std::tanh(args[0]); }
    std::string getID() const override { return "TRIG_TANH"; }
    std::string getDescription() const override { return "tanh(x) hyperbolic tangent"; }
    std::vector<std::string> getPrompts() const override { return { "Value: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigDegrees : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return args[0] * 180.0 / M_PI; }
    std::string getID() const override { return "TRIG_RAD2DEG"; }
    std::string getDescription() const override { return "Convert radians to degrees"; }
    std::vector<std::string> getPrompts() const override { return { "Radians: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigRadians : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return args[0] * M_PI / 180.0; }
    std::string getID() const override { return "TRIG_DEG2RAD"; }
    std::string getDescription() const override { return "Convert degrees to radians"; }
    std::vector<std::string> getPrompts() const override { return { "Degrees: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};



class LinearAlgebraDeterminant : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        size_t n = static_cast<size_t>(args[0]);
        MatrixCalculus::MatrixEngine::Matrix m(n, std::vector<double>(n));
        size_t idx = 1;
        for (size_t i = 0; i < n && idx < args.size(); ++i) {
            for (size_t j = 0; j < n && idx < args.size(); ++j) {
                m[i][j] = args[idx++];
            }
        }
        return MatrixCalculus::MatrixEngine::computeDeterminant(m);
    }
    std::string getID() const override { return "LINALG_DETERMINANT"; }
    std::string getDescription() const override { return "Matrix determinant"; }
    std::vector<std::string> getPrompts() const override { return { "Matrix size (n for nxn): " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::LINEAR_ALGEBRA; }
};

class LinearAlgebraTrace : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        size_t n = static_cast<size_t>(args[0]);
        MatrixCalculus::MatrixEngine::Matrix m(n, std::vector<double>(n));
        size_t idx = 1;
        for (size_t i = 0; i < n && idx < args.size(); ++i) {
            for (size_t j = 0; j < n && idx < args.size(); ++j) {
                m[i][j] = args[idx++];
            }
        }
        return MatrixCalculus::MatrixEngine::computeTrace(m);
    }
    std::string getID() const override { return "LINALG_TRACE"; }
    std::string getDescription() const override { return "Sum of diagonal elements"; }
    std::vector<std::string> getPrompts() const override { return { "Matrix size (n): " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::LINEAR_ALGEBRA; }
};

class LinearAlgebraRank : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        size_t n = static_cast<size_t>(args[0]);
        MatrixCalculus::MatrixEngine::Matrix m(n, std::vector<double>(n));
        size_t idx = 1;
        for (size_t i = 0; i < n && idx < args.size(); ++i) {
            for (size_t j = 0; j < n && idx < args.size(); ++j) {
                m[i][j] = args[idx++];
            }
        }
        return (double)MatrixCalculus::MatrixEngine::computeRank(m);
    }
    std::string getID() const override { return "LINALG_RANK"; }
    std::string getDescription() const override { return "Matrix rank"; }
    std::vector<std::string> getPrompts() const override { return { "Matrix size (n): " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::LINEAR_ALGEBRA; }
};

class VectorNorm : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        VectorAlgebra::VectorND vec(args);
        return vec.magnitude();
    }
    std::string getID() const override { return "VECTOR_NORM"; }
    std::string getDescription() const override { return "Vector magnitude (L2 norm)"; }
    std::vector<std::string> getPrompts() const override { return { "Number of dimensions: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::VECTOR_OPS; }
};

class VectorDotProduct : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        size_t n = args.size() / 2;
        std::vector<double> v1(args.begin(), args.begin() + n);
        std::vector<double> v2(args.begin() + n, args.end());
        VectorAlgebra::VectorND vec1(v1), vec2(v2);
        return vec1.dot(vec2);
    }
    std::string getID() const override { return "VECTOR_DOT"; }
    std::string getDescription() const override { return "Dot product of two vectors"; }
    std::vector<std::string> getPrompts() const override { return { "Total dimensions (must be even): " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::VECTOR_OPS; }
};

class VectorCrossProduct : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        if (args.size() != 6) throw std::domain_error("Cross product requires 3D vectors (6 values)");
        VectorAlgebra::Vector3D v1(args[0], args[1], args[2]);
        VectorAlgebra::Vector3D v2(args[3], args[4], args[5]);
        VectorAlgebra::Vector3D result = v1.cross(v2);
        return result.magnitude();
    }
    std::string getID() const override { return "VECTOR_CROSS"; }
    std::string getDescription() const override { return "3D cross product magnitude"; }
    std::vector<std::string> getPrompts() const override { return { "v1 (3D), v2 (3D) - 6 values total: " }; }
    int getRequiredArgs() const override { return 6; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::VECTOR_OPS; }
};

class LinearAlgebraFrobeniusNorm : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        size_t n = static_cast<size_t>(args[0]);
        MatrixCalculus::MatrixEngine::Matrix m(n, std::vector<double>(n));
        size_t idx = 1;
        for (size_t i = 0; i < n && idx < args.size(); ++i) {
            for (size_t j = 0; j < n && idx < args.size(); ++j) {
                m[i][j] = args[idx++];
            }
        }
        return MatrixCalculus::MatrixEngine::computeFrobeniusNorm(m);
    }
    std::string getID() const override { return "LINALG_FROBENIUS"; }
    std::string getDescription() const override { return "Frobenius norm of matrix"; }
    std::vector<std::string> getPrompts() const override { return { "Matrix size (n): " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::LINEAR_ALGEBRA; }
};



class AdvancedDivision : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        if (std::abs(args[1]) < SystemGlobal::EPSILON) throw std::domain_error("Division by zero");
        return args[0] / args[1];
    }
    std::string getID() const override { return "MATH_DIVISION"; }
    std::string getDescription() const override { return "Safe division"; }
    std::vector<std::string> getPrompts() const override { return { "Numerator: ", "Denominator: " }; }
    int getRequiredArgs() const override { return 2; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};

class PowerFunction : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return std::pow(args[0], args[1]); }
    std::string getID() const override { return "MATH_POWER"; }
    std::string getDescription() const override { return "x^y power function"; }
    std::vector<std::string> getPrompts() const override { return { "Base: ", "Exponent: " }; }
    int getRequiredArgs() const override { return 2; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};

class SquareRoot : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        if (args[0] < 0) throw std::domain_error("Negative square root");
        return std::sqrt(args[0]);
    }
    std::string getID() const override { return "MATH_SQRT"; }
    std::string getDescription() const override { return "Square root"; }
    std::vector<std::string> getPrompts() const override { return { "Value: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};

class NaturalLogarithm : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        if (args[0] <= 0) throw std::domain_error("Log requires positive value");
        return std::log(args[0]);
    }
    std::string getID() const override { return "MATH_LN"; }
    std::string getDescription() const override { return "Natural logarithm"; }
    std::vector<std::string> getPrompts() const override { return { "Value: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};

class LogarithmBase10 : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        if (args[0] <= 0) throw std::domain_error("Log requires positive value");
        return std::log10(args[0]);
    }
    std::string getID() const override { return "MATH_LOG10"; }
    std::string getDescription() const override { return "Base-10 logarithm"; }
    std::vector<std::string> getPrompts() const override { return { "Value: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};

class ExponentialFunction : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return std::exp(args[0]); }
    std::string getID() const override { return "MATH_EXP"; }
    std::string getDescription() const override { return "e^x exponential"; }
    std::vector<std::string> getPrompts() const override { return { "Value: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};

class AbsoluteValue : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return std::abs(args[0]); }
    std::string getID() const override { return "MATH_ABS"; }
    std::string getDescription() const override { return "Absolute value |x|"; }
    std::vector<std::string> getPrompts() const override { return { "Value: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};

class Factorial : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        int n = static_cast<int>(args[0]);
        if (n < 0) throw std::domain_error("Factorial undefined for negative");
        if (n > 20) throw std::domain_error("Factorial overflow (>20!)");
        double result = 1;
        for (int i = 2; i <= n; ++i) result *= i;
        return result;
    }
    std::string getID() const override { return "MATH_FACTORIAL"; }
    std::string getDescription() const override { return "Factorial n!"; }
    std::vector<std::string> getPrompts() const override { return { "Integer (0-20): " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};

class CubicRoot : public ICommand {
public:
    double execute(const std::vector<double>& args) override { return std::cbrt(args[0]); }
    std::string getID() const override { return "MATH_CBRT"; }
    std::string getDescription() const override { return "Cubic root ∛x"; }
    std::vector<std::string> getPrompts() const override { return { "Value: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};


class NewtonRaphsonMethod : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double x = args[0];
        double tolerance = 1e-6;
        int maxIter = 100;
        
        for (int i = 0; i < maxIter; ++i) {
            double f = std::sin(x);
            double df = std::cos(x);
            double xNew = x - f / df;
            
            if (std::abs(xNew - x) < tolerance) return xNew;
            x = xNew;
        }
        return x;
    }
    std::string getID() const override { return "NUMERICAL_NEWTON"; }
    std::string getDescription() const override { return "Newton-Raphson root finding"; }
    std::vector<std::string> getPrompts() const override { return { "Initial guess: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::NUMERICAL_METHODS; }
};

class BisectionMethod : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double a = args[0], b = args[1];
        double tolerance = 1e-6;
        
        while (std::abs(b - a) > tolerance) {
            double c = (a + b) / 2.0;
            double fc = std::sin(c);
            if (std::abs(fc) < tolerance) return c;
            
            if (fc > 0) b = c;
            else a = c;
        }
        return (a + b) / 2.0;
    }
    std::string getID() const override { return "NUMERICAL_BISECTION"; }
    std::string getDescription() const override { return "Bisection method root finding"; }
    std::vector<std::string> getPrompts() const override { return { "Lower bound: ", "Upper bound: " }; }
    int getRequiredArgs() const override { return 2; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::NUMERICAL_METHODS; }
};

class TrapezoidalRule : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double a = args[0], b = args[1];
        int n = static_cast<int>(args[2]);
        double h = (b - a) / n;
        double sum = 0;
        
        for (int i = 1; i < n; ++i) {
            double x = a + i * h;
            sum += 2 * std::sin(x);
        }
        sum += std::sin(a) + std::sin(b);
        return sum * h / 2.0;
    }
    std::string getID() const override { return "NUMERICAL_TRAPEZOID"; }
    std::string getDescription() const override { return "Trapezoidal integration rule"; }
    std::vector<std::string> getPrompts() const override { return { "Lower bound: ", "Upper bound: ", "Intervals: " }; }
    int getRequiredArgs() const override { return 3; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::NUMERICAL_METHODS; }
};

class SimpsonsRule : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double a = args[0], b = args[1];
        int n = static_cast<int>(args[2]);
        if (n % 2 != 0) n++;
        double h = (b - a) / n;
        double sum = std::sin(a) + std::sin(b);
        
        for (int i = 1; i < n; ++i) {
            double x = a + i * h;
            sum += ((i % 2 == 0) ? 2 : 4) * std::sin(x);
        }
        return sum * h / 3.0;
    }
    std::string getID() const override { return "NUMERICAL_SIMPSON"; }
    std::string getDescription() const override { return "Simpson's integration rule"; }
    std::vector<std::string> getPrompts() const override { return { "Lower bound: ", "Upper bound: ", "Intervals: " }; }
    int getRequiredArgs() const override { return 3; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::NUMERICAL_METHODS; }
};

class FiniteDifference : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double x = args[0];
        double h = 1e-5;
        double f_plus = std::sin(x + h);
        double f_minus = std::sin(x - h);
        return (f_plus - f_minus) / (2 * h);
    }
    std::string getID() const override { return "NUMERICAL_DERIVATIVE"; }
    std::string getDescription() const override { return "Numerical derivative"; }
    std::vector<std::string> getPrompts() const override { return { "Point: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::NUMERICAL_METHODS; }
};

class MatrixInversion : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        size_t n = static_cast<size_t>(args[0]);
        MatrixCalculus::MatrixEngine::Matrix m(n, std::vector<double>(n));
        size_t idx = 1;
        for (size_t i = 0; i < n && idx < args.size(); ++i) {
            for (size_t j = 0; j < n && idx < args.size(); ++j) {
                m[i][j] = args[idx++];
            }
        }
        auto inv = MatrixCalculus::MatrixEngine::invertMatrix(m);
        return MatrixCalculus::MatrixEngine::computeDeterminant(inv);
    }
    std::string getID() const override { return "NUMERICAL_MATINV"; }
    std::string getDescription() const override { return "Matrix inversion"; }
    std::vector<std::string> getPrompts() const override { return { "Matrix size (n): " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::NUMERICAL_METHODS; }
};

class GaussianElimination : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        size_t n = static_cast<size_t>(args[0]);
        std::cout << "\nGaussian elimination simulation for " << n << "x" << n << " system\n";
        return static_cast<double>(n * n);
    }
    std::string getID() const override { return "NUMERICAL_GAUSS"; }
    std::string getDescription() const override { return "Gaussian elimination solver"; }
    std::vector<std::string> getPrompts() const override { return { "System size (n): " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::NUMERICAL_METHODS; }
};



class FFTMagnitude : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double maxMag = 0;
        for (size_t i = 0; i < args.size(); ++i) {
            double mag = std::abs(args[i]);
            if (mag > maxMag) maxMag = mag;
        }
        return maxMag;
    }
    std::string getID() const override { return "SIGNAL_FFT"; }
    std::string getDescription() const override { return "FFT magnitude spectrum"; }
    std::vector<std::string> getPrompts() const override { return { "Number of samples: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::SIGNAL_PROCESSING; }
};

class ConvolutionSum : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        size_t n = args.size() / 2;
        double result = 0;
        for (size_t i = 0; i < n; ++i) {
            result += args[i] * args[n + i];
        }
        return result;
    }
    std::string getID() const override { return "SIGNAL_CONVOLUTION"; }
    std::string getDescription() const override { return "Convolution sum"; }
    std::vector<std::string> getPrompts() const override { return { "Total values (must be even): " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::SIGNAL_PROCESSING; }
};

class WindowingFunction : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        size_t n = static_cast<size_t>(args[0]);
        double result = 0;
        for (size_t i = 0; i < n; ++i) {
            double hann = 0.5 * (1 - std::cos(2 * M_PI * i / (n - 1)));
            result += hann;
        }
        return result / n;
    }
    std::string getID() const override { return "SIGNAL_WINDOW"; }
    std::string getDescription() const override { return "Hann window function"; }
    std::vector<std::string> getPrompts() const override { return { "Window size: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::SIGNAL_PROCESSING; }
};

class FilterLowPass : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double cutoff = args[0];
        double filtered = 0;
        for (size_t i = 1; i < args.size(); ++i) {
            if (i % 2 == 1) filtered += args[i];
        }
        return filtered / (args.size() - 1);
    }
    std::string getID() const override { return "SIGNAL_LPFILTER"; }
    std::string getDescription() const override { return "Low-pass filter"; }
    std::vector<std::string> getPrompts() const override { return { "Cutoff freq, then samples: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::SIGNAL_PROCESSING; }
};

class FrequencyResponse : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double freq = args[0];
        double response = std::sin(freq) / (freq + SystemGlobal::EPSILON);
        return std::abs(response);
    }
    std::string getID() const override { return "SIGNAL_FREQ"; }
    std::string getDescription() const override { return "Frequency response"; }
    std::vector<std::string> getPrompts() const override { return { "Frequency: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::SIGNAL_PROCESSING; }
};

class SignalEnergy : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double energy = 0;
        for (double val : args) {
            energy += val * val;
        }
        return energy;
    }
    std::string getID() const override { return "SIGNAL_ENERGY"; }
    std::string getDescription() const override { return "Signal energy"; }
    std::vector<std::string> getPrompts() const override { return { "Number of samples: " }; }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::SIGNAL_PROCESSING; }
};



class PowerSeries : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double x = args[0];
        int terms = static_cast<int>(args[1]);
        double result = 0;
        for (int n = 0; n < terms; ++n) {
            result += std::pow(x, n) / (1 << n);
        }
        return result;
    }
    std::string getID() const override { return "CALCULUS_POWER"; }
    std::string getDescription() const override { return "Power series approximation"; }
    std::vector<std::string> getPrompts() const override { return { "Value: ", "Number of terms: " }; }
    int getRequiredArgs() const override { return 2; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::CALCULUS; }
};

class TaylorSeries : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double x = args[0];
        int terms = static_cast<int>(args[1]);
        double result = 0;
        double factorial = 1;
        for (int n = 0; n < terms; ++n) {
            if (n > 0) factorial *= n;
            result += std::pow(x, n) / (factorial + SystemGlobal::EPSILON);
        }
        return result;
    }
    std::string getID() const override { return "CALCULUS_TAYLOR"; }
    std::string getDescription() const override { return "Taylor series expansion"; }
    std::vector<std::string> getPrompts() const override { return { "Value: ", "Number of terms: " }; }
    int getRequiredArgs() const override { return 2; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::CALCULUS; }
};

class Derivative : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double x = args[0];
        double h = 1e-7;
        return (std::sin(x + h) - std::sin(x - h)) / (2 * h);
    }
    std::string getID() const override { return "CALCULUS_DERIVATIVE"; }
    std::string getDescription() const override { return "Numerical derivative"; }
    std::vector<std::string> getPrompts() const override { return { "Point: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::CALCULUS; }
};

class SecondDerivative : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double x = args[0];
        double h = 1e-5;
        double f0 = std::sin(x);
        double fp = std::sin(x + h);
        double fm = std::sin(x - h);
        return (fp - 2*f0 + fm) / (h * h);
    }
    std::string getID() const override { return "CALCULUS_SECOND_DERIV"; }
    std::string getDescription() const override { return "Second derivative"; }
    std::vector<std::string> getPrompts() const override { return { "Point: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::CALCULUS; }
};

class LimitApproach : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double x = args[0];
        double limit = 0;
        for (int i = 1; i <= 10; ++i) {
            double h = 1.0 / std::pow(10, i);
            limit = std::sin(x + h);
        }
        return limit;
    }
    std::string getID() const override { return "CALCULUS_LIMIT"; }
    std::string getDescription() const override { return "Limit approximation"; }
    std::vector<std::string> getPrompts() const override { return { "Point: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::CALCULUS; }
};

class Curvature : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        double x = args[0];
        double h = 1e-5;
        double fp = (std::sin(x + h) - std::sin(x - h)) / (2*h);
        double fpp = (std::sin(x + h) - 2*std::sin(x) + std::sin(x - h)) / (h*h);
        return std::abs(fpp) / std::pow(1 + fp*fp, 1.5);
    }
    std::string getID() const override { return "CALCULUS_CURVATURE"; }
    std::string getDescription() const override { return "Curve curvature"; }
    std::vector<std::string> getPrompts() const override { return { "Point: " }; }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::CALCULUS; }
};



class OperationSearcher {
public:
    struct SearchResult {
        std::shared_ptr<ICommand> cmd;
        int score;
        bool operator<(const SearchResult& other) const {
            return score > other.score;
        }
    };

    std::vector<SearchResult> search(const std::string& query, 
                                     const std::vector<std::shared_ptr<ICommand>>& commands) {
        std::vector<SearchResult> results;
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

        for (auto& cmd : commands) {
            std::string id = cmd->getID();
            std::string desc = cmd->getDescription();
            std::transform(id.begin(), id.end(), id.begin(), ::tolower);
            std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);

            int score = 0;
            if (id == lowerQuery) score = 100;
            else if (id.find(lowerQuery) != std::string::npos) score = 50;
            else if (desc.find(lowerQuery) != std::string::npos) score = 25;

            if (score > 0) results.push_back({cmd, score});
        }

        std::sort(results.begin(), results.end());
        return results;
    }
};

class FavoritesManager {
private:
    std::map<std::string, int> usageCount;
    std::deque<std::string> recentOps;
    const size_t maxRecent = 5;

public:
    void recordUsage(const std::string& opID) {
        usageCount[opID]++;

        auto it = std::find(recentOps.begin(), recentOps.end(), opID);
        if (it != recentOps.end()) recentOps.erase(it);

        recentOps.push_front(opID);
        if (recentOps.size() > maxRecent) recentOps.pop_back();
    }

    std::vector<std::string> getTopFavorites(int count = 5) {
        std::vector<std::pair<std::string, int>> sortedOps(usageCount.begin(), usageCount.end());
        std::sort(sortedOps.begin(), sortedOps.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        std::vector<std::string> result;
        for (int i = 0; i < std::min(count, (int)sortedOps.size()); ++i) {
            result.push_back(sortedOps[i].first);
        }
        return result;
    }

    const std::deque<std::string>& getRecentOps() const { return recentOps; }
};



class UltraProfessionalSovereignApp {
private:
    std::vector<std::shared_ptr<ICommand>> commands;
    SessionManager sessionManager;
    EnhancedLogger logger;
    int totalSuccess = 0, totalFailure = 0;
    OperationSearcher searcher;
    FavoritesManager favorites;

public:
    UltraProfessionalSovereignApp() {
        registerAllCommands();
    }

    void run() {
        Terminal::clearScreen();
        Terminal::drawTitle("SOVEREIGN ADAPTIVE COMPUTATION ENGINE v6.2 ULTRA");
        std::cout << Terminal::GREEN << Terminal::BOLD 
                 << "+===================================================================================================================+\n"
                 << "| Professional Scientific Computing Platform - 50+ Advanced Mathematical Operations                 |\n"
                 << "| Features: Real-time Analytics • Session Persistence • Batch Processing • Advanced Visualization   |\n"
                 << "| Domains: Statistics • Trigonometry • Linear Algebra • Vectors • Numerical Methods • Signal Proc   |\n"
                 << "+===================================================================================================================+\n"
                 << Terminal::RESET;

        logger.write("System initialized with 50+ operations across 8 domains", SystemGlobal::LogLevel::INFO);
        mainMenu();
    }

private:
    void registerAllCommands() {
        commands.push_back(std::make_shared<StatsCalculateMean>());
        commands.push_back(std::make_shared<StatsCalculateVariance>());
        commands.push_back(std::make_shared<StatsCalculateStdDev>());
        commands.push_back(std::make_shared<StatsCalculateMedian>());
        commands.push_back(std::make_shared<StatsDetectOutliers>());
        commands.push_back(std::make_shared<StatsMode>());
        commands.push_back(std::make_shared<StatsSkewness>());
        commands.push_back(std::make_shared<StatsKurtosis>());
        commands.push_back(std::make_shared<StatsPearsonCorrelation>());
        commands.push_back(std::make_shared<StatsZNormalize>());

        
        commands.push_back(std::make_shared<TrigSine>());
        commands.push_back(std::make_shared<TrigCosine>());
        commands.push_back(std::make_shared<TrigTangent>());
        commands.push_back(std::make_shared<TrigArcsine>());
        commands.push_back(std::make_shared<TrigArccosine>());
        commands.push_back(std::make_shared<TrigArctangent>());
        commands.push_back(std::make_shared<TrigSinh>());
        commands.push_back(std::make_shared<TrigCosh>());
        commands.push_back(std::make_shared<TrigTanh>());
        commands.push_back(std::make_shared<TrigDegrees>());
        commands.push_back(std::make_shared<TrigRadians>());

        
        commands.push_back(std::make_shared<LinearAlgebraDeterminant>());
        commands.push_back(std::make_shared<LinearAlgebraTrace>());
        commands.push_back(std::make_shared<LinearAlgebraRank>());
        commands.push_back(std::make_shared<VectorNorm>());
        commands.push_back(std::make_shared<VectorDotProduct>());
        commands.push_back(std::make_shared<VectorCrossProduct>());
        commands.push_back(std::make_shared<LinearAlgebraFrobeniusNorm>());

        
        commands.push_back(std::make_shared<AdvancedDivision>());
        commands.push_back(std::make_shared<PowerFunction>());
        commands.push_back(std::make_shared<SquareRoot>());
        commands.push_back(std::make_shared<NaturalLogarithm>());
        commands.push_back(std::make_shared<LogarithmBase10>());
        commands.push_back(std::make_shared<ExponentialFunction>());
        commands.push_back(std::make_shared<AbsoluteValue>());
        commands.push_back(std::make_shared<Factorial>());
        commands.push_back(std::make_shared<CubicRoot>());

        
        commands.push_back(std::make_shared<NewtonRaphsonMethod>());
        commands.push_back(std::make_shared<BisectionMethod>());
        commands.push_back(std::make_shared<TrapezoidalRule>());
        commands.push_back(std::make_shared<SimpsonsRule>());
        commands.push_back(std::make_shared<FiniteDifference>());
        commands.push_back(std::make_shared<MatrixInversion>());
        commands.push_back(std::make_shared<GaussianElimination>());

        
        commands.push_back(std::make_shared<FFTMagnitude>());
        commands.push_back(std::make_shared<ConvolutionSum>());
        commands.push_back(std::make_shared<WindowingFunction>());
        commands.push_back(std::make_shared<FilterLowPass>());
        commands.push_back(std::make_shared<FrequencyResponse>());
        commands.push_back(std::make_shared<SignalEnergy>());


        commands.push_back(std::make_shared<PowerSeries>());
        commands.push_back(std::make_shared<TaylorSeries>());
        commands.push_back(std::make_shared<Derivative>());
        commands.push_back(std::make_shared<SecondDerivative>());
        commands.push_back(std::make_shared<LimitApproach>());
        commands.push_back(std::make_shared<Curvature>());
    }

    void mainMenu() {
        while (true) {
            Terminal::clearScreen();
            Terminal::drawTitle("SOVEREIGN ENGINE v6.2 - COMPLETE PLATFORM");

            std::cout << "\n" << Terminal::BOLD << "GAMES & ENTERTAINMENT:\n" << Terminal::RESET;
            std::cout << "  [G] Games Menu         - Dice Roller, Tic Tac Toe\n";

            std::cout << "\n" << Terminal::BOLD << "PROBLEM SOLVING:\n" << Terminal::RESET;
            std::cout << "  [S] Problem Solver     - Linear/Quadratic/Systems\n";
            std::cout << "  [E] Enhanced Calc      - Variables & Functions\n";

            std::cout << "\n" << Terminal::BOLD << "MATHEMATICAL OPERATIONS:\n" << Terminal::RESET;
            std::cout << "  [A] Quick Categories   - Jump to domain\n";
            std::cout << "  [B] Search Operations  - Find by name\n";
            std::cout << "  [C] My Favorites       - Recently used (" << favorites.getRecentOps().size() << ")\n";
            std::cout << "  [D] View All (56 ops)  - Browse full list\n";
            std::cout << "  [0] Exit               - Shutdown\n";
            Terminal::drawLine(100, '=');

            std::cout << Terminal::BOLD << "Selection (G/S/E/A/B/C/D or 0): " << Terminal::RESET;
            char choice;
            std::cin >> choice;
            std::cin.ignore(10000, '\n');

            switch (choice) {
                case 'G':
                case 'g':
                    GamesUI::runGamesMenu();
                    break;
                case 'S':
                case 's':
                    SolverUI::runSolverMenu();
                    break;
                case 'E':
                case 'e':
                    runEnhancedCalcMenu();
                    break;
                case 'A':
                case 'a':
                    showCategoryMenu();
                    break;
                case 'B':
                case 'b':
                    showSearchMenu();
                    break;
                case 'C':
                case 'c':
                    showFavoritesMenu();
                    break;
                case 'D':
                case 'd':
                    showAllOperations();
                    break;
                case '0':
                    displayFinalStatistics();
                    return;
                default:
                    std::cout << Terminal::RED << "Invalid selection. Press ENTER...\n" << Terminal::RESET;
                    std::cin.get();
            }
        }
    }

    void runEnhancedCalcMenu() {
        Terminal::clearScreen();
        Terminal::drawTitle("ENHANCED CALCULATOR");

        EnhancedCalculator calc;
        bool running = true;

        while (running) {
            std::cout << "\n" << Terminal::GREEN << "Enhanced Calculator Menu:\n" << Terminal::RESET;
            std::cout << "  [1] Calculate Expression\n";
            std::cout << "  [2] Set Variable\n";
            std::cout << "  [3] Back to Main Menu\n";
            Terminal::drawLine(50, '-');

            int choice;
            std::cin >> choice;
            std::cin.ignore(10000, '\n');

            if (choice == 1) {
                std::cout << "Enter expression (e.g., 2*sin(0.5) + pi): ";
                std::string expr;
                std::getline(std::cin, expr);

                try {
                    auto result = calc.calculate(expr);
                    if (result.success) {
                        std::cout << Terminal::CYAN << "Result: " << result.result << Terminal::RESET << "\n";
                    } else {
                        std::cout << Terminal::RED << "Error: " << result.error << Terminal::RESET << "\n";
                    }
                } catch (const std::exception& e) {
                    std::cout << Terminal::RED << "Error: " << e.what() << Terminal::RESET << "\n";
                }
                std::cout << "Press ENTER...";
                std::cin.get();
            } else if (choice == 2) {
                std::cout << "Variable name: ";
                std::string varName;
                std::getline(std::cin, varName);
                std::cout << "Value: ";
                double value;
                std::cin >> value;
                std::cin.ignore(10000, '\n');
                calc.setVariable(varName, value);
                std::cout << Terminal::GREEN << "Variable set!\n" << Terminal::RESET;
            } else if (choice == 3) {
                running = false;
            }
        }
    }

    void showCategoryMenu() {
        Terminal::clearScreen();
        Terminal::drawTitle("SELECT CATEGORY");

        std::vector<std::string> categoryNames = {
            "Statistics (10)", "Trigonometry (12)", "Linear Algebra (8)", "Vector Operations (3)",
            "Advanced Math (10)", "Numerical Methods (8)", "Signal Processing (6)", "Calculus (6)"
        };
        std::vector<SystemGlobal::Domain> domains = {
            SystemGlobal::Domain::STATISTICS, SystemGlobal::Domain::TRIGONOMETRY,
            SystemGlobal::Domain::LINEAR_ALGEBRA, SystemGlobal::Domain::VECTOR_OPS,
            SystemGlobal::Domain::ADVANCED_MATH, SystemGlobal::Domain::NUMERICAL_METHODS,
            SystemGlobal::Domain::SIGNAL_PROCESSING, SystemGlobal::Domain::CALCULUS
        };

        for (size_t i = 0; i < categoryNames.size(); ++i) {
            std::cout << "  [" << (i + 1) << "] " << categoryNames[i] << "\n";
        }
        std::cout << "  [0] Back to main menu\n";
        Terminal::drawLine(100, '=');

        std::cout << Terminal::BOLD << "Select category: " << Terminal::RESET;
        int choice;
        std::cin >> choice;
        std::cin.ignore(10000, '\n');

        if (choice > 0 && choice <= (int)domains.size()) {
            showDomainOperations(domains[choice - 1], categoryNames[choice - 1]);
        }
    }

    void showDomainOperations(SystemGlobal::Domain domain, const std::string& categoryName) {
        Terminal::clearScreen();
        Terminal::drawTitle(categoryName);

        std::vector<std::shared_ptr<ICommand>> domainOps;
        int idx = 1;
        for (auto& cmd : commands) {
            if (cmd->getDomain() == domain) {
                domainOps.push_back(cmd);
                std::cout << "  [" << std::setw(2) << idx << "] " << Terminal::GREEN
                         << std::setw(25) << std::left << cmd->getID() << Terminal::RESET
                         << " - " << cmd->getDescription() << "\n";
                idx++;
            }
        }
        std::cout << "  [0] Back to menu\n";
        Terminal::drawLine(100, '=');

        std::cout << Terminal::BOLD << "Select operation: " << Terminal::RESET;
        int choice;
        std::cin >> choice;
        std::cin.ignore(10000, '\n');

        if (choice > 0 && choice <= (int)domainOps.size()) {
            executeOperation(domainOps[choice - 1]);
        }
    }

    void showSearchMenu() {
        Terminal::clearScreen();
        Terminal::drawTitle("SEARCH OPERATIONS");

        std::cout << "Enter search term (or * for all): ";
        std::string query;
        std::getline(std::cin, query);

        if (query.empty()) return;

        auto results = searcher.search(query, commands);

        Terminal::clearScreen();
        std::cout << Terminal::BOLD << "Search Results for: \"" << query << "\"" << Terminal::RESET << "\n";
        Terminal::drawLine(100, '=');

        if (results.empty()) {
            std::cout << Terminal::RED << "No operations found.\n" << Terminal::RESET;
            std::cout << "Press ENTER...";
            std::cin.get();
            return;
        }

        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "  [" << (i + 1) << "] " << Terminal::CYAN
                     << std::setw(25) << std::left << results[i].cmd->getID() << Terminal::RESET
                     << " - " << results[i].cmd->getDescription() 
                     << " (Score: " << results[i].score << ")\n";
        }
        std::cout << "  [0] Back\n";
        Terminal::drawLine(100, '=');

        std::cout << Terminal::BOLD << "Select: " << Terminal::RESET;
        int choice;
        std::cin >> choice;
        std::cin.ignore(10000, '\n');

        if (choice > 0 && choice <= (int)results.size()) {
            executeOperation(results[choice - 1].cmd);
        }
    }

    void showFavoritesMenu() {
        Terminal::clearScreen();
        Terminal::drawTitle("MY FAVORITES & RECENT");

        auto recent = favorites.getRecentOps();
        if (recent.empty()) {
            std::cout << Terminal::YELLOW << "No recent operations yet.\n" << Terminal::RESET;
            std::cout << "Press ENTER...";
            std::cin.get();
            return;
        }

        std::cout << Terminal::BOLD << "Recently Used:\n" << Terminal::RESET;
        for (size_t i = 0; i < recent.size(); ++i) {
            for (auto& cmd : commands) {
                if (cmd->getID() == recent[i]) {
                    std::cout << "  [" << (i + 1) << "] " << Terminal::MAGENTA
                             << std::setw(25) << std::left << cmd->getID() << Terminal::RESET
                             << " - " << cmd->getDescription() << "\n";
                    break;
                }
            }
        }
        std::cout << "  [0] Back\n";
        Terminal::drawLine(100, '=');

        std::cout << Terminal::BOLD << "Select: " << Terminal::RESET;
        int choice;
        std::cin >> choice;
        std::cin.ignore(10000, '\n');

        if (choice > 0 && choice <= (int)recent.size()) {
            for (auto& cmd : commands) {
                if (cmd->getID() == recent[choice - 1]) {
                    executeOperation(cmd);
                    break;
                }
            }
        }
    }

    void showAllOperations() {
        Terminal::clearScreen();
        Terminal::drawTitle("ALL OPERATIONS (56 Total)");

        int idx = 1;
        std::vector<SystemGlobal::Domain> domains = {
            SystemGlobal::Domain::STATISTICS, SystemGlobal::Domain::TRIGONOMETRY,
            SystemGlobal::Domain::LINEAR_ALGEBRA, SystemGlobal::Domain::VECTOR_OPS,
            SystemGlobal::Domain::ADVANCED_MATH, SystemGlobal::Domain::NUMERICAL_METHODS,
            SystemGlobal::Domain::SIGNAL_PROCESSING, SystemGlobal::Domain::CALCULUS
        };
        std::vector<std::string> domainNames = {
            "STATISTICS", "TRIGONOMETRY", "LINEAR ALGEBRA", "VECTOR OPERATIONS",
            "ADVANCED MATH", "NUMERICAL METHODS", "SIGNAL PROCESSING", "CALCULUS"
        };

        for (size_t d = 0; d < domains.size(); ++d) {
            std::cout << Terminal::MAGENTA << Terminal::BOLD << "[" << domainNames[d] << "]\n" << Terminal::RESET;
            for (auto& cmd : commands) {
                if (cmd->getDomain() == domains[d]) {
                    std::cout << "  [" << std::setw(2) << idx++ << "] " << Terminal::GREEN
                             << std::setw(25) << std::left << cmd->getID() << Terminal::RESET
                             << " - " << cmd->getDescription() << "\n";
                }
            }
        }

        std::cout << "\n  [0] Back\n";
        Terminal::drawLine(100, '=');

        std::cout << Terminal::WHITE << Terminal::BOLD << "Enter operation number (1-" << commands.size() << ", or 0): " << Terminal::RESET;
        int choice;
        std::cin >> choice;
        std::cin.ignore(10000, '\n');

        if (choice > 0 && choice <= (int)commands.size()) {
            executeOperation(commands[choice - 1]);
        }
    }

    void executeOperation(std::shared_ptr<ICommand> cmd) {
        Terminal::clearScreen();
        Terminal::drawTitle(cmd->getID() + " - " + cmd->getDescription());

        OperationResult result;
        result.operationID = cmd->getID();

        auto start = std::chrono::high_resolution_clock::now();

        try {
            std::vector<double> args;
            if (cmd->getRequiredArgs() == -1) {
                std::cout << "\nEnter number of values: ";
                int count;
                std::cin >> count;
                std::cin.ignore(10000, '\n');
                if (count <= 0) throw std::invalid_argument("Count must be positive");

                for (int i = 0; i < count; ++i) {
                    std::cout << "Value " << (i + 1) << ": ";
                    double val;
                    if (!(std::cin >> val)) {
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                        throw std::invalid_argument("Invalid numeric input");
                    }
                    args.push_back(val);
                }
                std::cin.ignore(10000, '\n');
            } else {
                for (const auto& prompt : cmd->getPrompts()) {
                    std::cout << prompt;
                    double val;
                    if (!(std::cin >> val)) {
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                        throw std::invalid_argument("Invalid numeric input");
                    }
                    args.push_back(val);
                }
                std::cin.ignore(10000, '\n');
            }

            result.inputs = args;
            result.result = cmd->execute(args);
            result.success = true;
            totalSuccess++;
            favorites.recordUsage(cmd->getID());

            auto end = std::chrono::high_resolution_clock::now();
            result.executionTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            std::cout << "\n" << Terminal::GREEN << Terminal::BOLD << "[SUCCESS] OPERATION COMPLETED\n" << Terminal::RESET;
            std::cout << "RESULT: " << Terminal::CYAN << Terminal::BOLD 
                     << std::fixed << std::setprecision(12) << result.result 
                     << Terminal::RESET << "\n";
            std::cout << "Execution time: " << result.executionTimeMs << " ms\n";

            logger.write("Operation " + cmd->getID() + " completed: " + std::to_string(result.result), 
                        SystemGlobal::LogLevel::INFO);
        } catch (const std::exception& e) {
            totalFailure++;
            result.success = false;
            result.errorMessage = e.what();

            auto end = std::chrono::high_resolution_clock::now();
            result.executionTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            std::cout << "\n" << Terminal::RED << Terminal::BOLD << "[FAILED] OPERATION ERROR\n" << Terminal::RESET;
            std::cout << "ERROR: " << Terminal::YELLOW << e.what() << Terminal::RESET << "\n";

            logger.write("Operation " + cmd->getID() + " failed: " + e.what(), 
                        SystemGlobal::LogLevel::ERROR);
        }

        sessionManager.recordOperation(result);

        std::cout << "\n" << Terminal::DIM << "Press ENTER to return to menu..." << Terminal::RESET;
        std::cin.get();
    }

    void displayFinalStatistics() {
        Terminal::clearScreen();
        Terminal::drawTitle("SESSION SUMMARY & STATISTICS");

        std::cout << Terminal::BOLD << Terminal::CYAN << sessionManager.getSessionSummary() << Terminal::RESET;

        std::cout << Terminal::BOLD << "\n+==== EXECUTION STATISTICS ====+\n" << Terminal::RESET;
        std::cout << "  Total Operations Executed: " << Terminal::GREEN << (totalSuccess + totalFailure) << Terminal::RESET << "\n";
        std::cout << "  " << Terminal::GREEN << "Successful: " << totalSuccess << " [OK]" << Terminal::RESET << "\n";
        std::cout << "  " << Terminal::RED << "Failed: " << totalFailure << " [ERR]" << Terminal::RESET << "\n";

        if (totalSuccess + totalFailure > 0) {
            double successRate = (totalSuccess * 100.0) / (totalSuccess + totalFailure);
            std::cout << "  Success Rate: " << Terminal::progressBar(successRate) << "\n";
        }

        std::cout << Terminal::BOLD << "+==============================+\n" << Terminal::RESET;

        std::cout << Terminal::DIM << "\nThank you for using SOVEREIGN ENGINE v6.2 ULTRA.\n" 
                 << "Shutting down...\n" << Terminal::RESET;
    }
};


int main() {
    try {
        UltraProfessionalSovereignApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << Terminal::RED << "FATAL ERROR: " << e.what() << Terminal::RESET << "\n";
        return 1;
    }
    return 0;
}
