

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <iomanip>
#include <stdexcept>
#include <memory>
#include <map>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <sstream>
#include "linearalgebra.cpp"


namespace SystemGlobal {
    const std::string VERSION_ID = "6.2.0-PRODUCTION";
    const std::string LOG_PATH = "sovereign_engine.log";
    const double EPSILON = 1e-9;

    enum class LogLevel { INFO, WARNING, ERROR, CRITICAL };
    enum class Domain { 
        STATISTICS, 
        TRIGONOMETRY, 
        LINEAR_ALGEBRA, 
        VECTOR_OPS, 
        ADVANCED_MATH 
    };

    struct EngineConfig {
        bool enableLogging = true;
        bool enableHeuristics = true;
        bool verboseOutput = true;
        int maxIterations = 100;
        double convergenceTolerance = 1e-6;
    };
}

class SystemLogger {
private:
    static std::ofstream logFile;

public:
    static void initialize() {
        if (SystemGlobal::EngineConfig().enableLogging) {
            logFile.open(SystemGlobal::LOG_PATH, std::ios::app);
        }
    }

    static void write(const std::string& message, SystemGlobal::LogLevel level) {
        std::string prefix, color;
        switch (level) {
        case SystemGlobal::LogLevel::INFO:      prefix = "[INFO]";    color = "\033[92m"; break;
        case SystemGlobal::LogLevel::WARNING:   prefix = "[WARN]";    color = "\033[93m"; break;
        case SystemGlobal::LogLevel::ERROR:     prefix = "[ERROR]";   color = "\033[91m"; break;
        case SystemGlobal::LogLevel::CRITICAL:  prefix = "[FATAL]";   color = "\033[95m"; break;
        }

        std::string timestamp = getTimestamp();
        std::string output = prefix + " " + message;

        std::cout << color << output << "\033[0m" << std::endl;

        if (logFile.is_open()) {
            logFile << timestamp << " " << output << std::endl;
            logFile.flush();
        }
    }

private:
    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;

        #ifdef _MSC_VER
            struct tm tm_info;
            localtime_s(&tm_info, &time);
            ss << "[" << std::put_time(&tm_info, "%Y-%m-%d %H:%M:%S") << "]";
        #else
            ss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "]";
        #endif

        return ss.str();
    }
};
std::ofstream SystemLogger::logFile;

class AdaptiveBrain {
private:
    struct OperationRecord {
        int successCount = 0;
        int failureCount = 0;
        std::string lastError;
        std::chrono::system_clock::time_point lastAttempt;
        bool isOptimal = false;
    };
    std::map<std::string, OperationRecord> operationHistory;

public:
    void recordSuccess(const std::string& opID) {
        operationHistory[opID].successCount++;
        operationHistory[opID].lastAttempt = std::chrono::system_clock::now();

        if (operationHistory[opID].successCount + operationHistory[opID].failureCount > 0) {
            float successRate = operationHistory[opID].successCount / 
                (float)(operationHistory[opID].successCount + operationHistory[opID].failureCount);

            if (successRate > 0.95) {
                operationHistory[opID].isOptimal = true;
            }
        }
    }

    void recordFailure(const std::string& opID, const std::string& reason) {
        operationHistory[opID].failureCount++;
        operationHistory[opID].lastError = reason;
        operationHistory[opID].lastAttempt = std::chrono::system_clock::now();

        SystemLogger::write("Operation " + opID + " failed: " + reason, SystemGlobal::LogLevel::WARNING);
    }

    void provideSuggestion(const std::string& opID) {
        if (!SystemGlobal::EngineConfig().enableHeuristics) return;

        auto& record = operationHistory[opID];

        if (record.failureCount >= 2) {
            std::cout << "\n" << std::string(60, '=') << "\n";
            std::cout << ">>> [ADAPTIVE ASSISTANT]: Operation '" << opID << "' requires attention.\n";
            float rate = (record.successCount + record.failureCount > 0) ? 
                (record.successCount / (float)(record.successCount + record.failureCount) * 100) : 0;
            std::cout << ">>> Success Rate: " << std::fixed << std::setprecision(1) << rate << "%\n";
            std::cout << ">>> Last Error: " << record.lastError << "\n";
            std::cout << ">>> Recommendation: Verify input parameters and ranges.\n";
            std::cout << std::string(60, '=') << "\n";
        }
    }

    std::string getStatistics() const {
        std::stringstream ss;
        ss << "\n=== OPERATION STATISTICS ===\n";
        for (const auto& [opID, record] : operationHistory) {
            int total = record.successCount + record.failureCount;
            float rate = total > 0 ? (record.successCount / (float)total * 100) : 0;
            ss << opID << ": " << record.successCount << "✓ / " 
               << record.failureCount << "✗ (" << std::fixed << std::setprecision(1) 
               << rate << "%)\n";
        }
        return ss.str();
    }
};


class ICommand {
public:
    virtual ~ICommand() = default;
    virtual double execute(const std::vector<double>& args) = 0;
    virtual std::string getID() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getPrompts() const = 0;
    virtual int getRequiredArgs() const = 0;
    virtual SystemGlobal::Domain getDomain() const = 0;
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
    std::vector<std::string> getPrompts() const override { 
        return { "How many values? Enter count: " }; 
    }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};

class StatsCalculateVariance : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        Statistics::Dataset data;
        for (double val : args) data.addObservation(val);
        return data.calculateVariance();
    }
    std::string getID() const override { return "STATS_VARIANCE"; }
    std::string getDescription() const override { return "Calculate population variance"; }
    std::vector<std::string> getPrompts() const override { 
        return { "How many values? Enter count: " }; 
    }
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
    std::string getDescription() const override { return "Calculate standard deviation"; }
    std::vector<std::string> getPrompts() const override { 
        return { "How many values? Enter count: " }; 
    }
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
    std::vector<std::string> getPrompts() const override { 
        return { "How many values? Enter count: " }; 
    }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};

class StatsDetectOutliers : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        Statistics::Dataset data;
        for (size_t i = 0; i < args.size() - 1; ++i) 
            data.addObservation(args[i]);
        double method = args.back();

        std::vector<double> outliers;
        if (method < 1.5) outliers = data.detectOutliersIQR();
        else outliers = data.detectOutliersZScore(3.0);

        std::cout << "\nDetected " << outliers.size() << " outliers: ";
        for (double o : outliers) std::cout << o << " ";
        std::cout << "\n";

        return (double)outliers.size();
    }
    std::string getID() const override { return "STATS_OUTLIERS"; }
    std::string getDescription() const override { return "Detect outliers (IQR or Z-Score)"; }
    std::vector<std::string> getPrompts() const override { 
        return { "How many values? Enter count: " }; 
    }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::STATISTICS; }
};



class TrigSine : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        return std::sin(args[0]);
    }
    std::string getID() const override { return "TRIG_SINE"; }
    std::string getDescription() const override { return "Calculate sine (radians)"; }
    std::vector<std::string> getPrompts() const override { 
        return { "Enter angle (radians): " }; 
    }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigCosine : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        return std::cos(args[0]);
    }
    std::string getID() const override { return "TRIG_COSINE"; }
    std::string getDescription() const override { return "Calculate cosine (radians)"; }
    std::vector<std::string> getPrompts() const override { 
        return { "Enter angle (radians): " }; 
    }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::TRIGONOMETRY; }
};

class TrigTangent : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        return std::tan(args[0]);
    }
    std::string getID() const override { return "TRIG_TANGENT"; }
    std::string getDescription() const override { return "Calculate tangent (radians)"; }
    std::vector<std::string> getPrompts() const override { 
        return { "Enter angle (radians): " }; 
    }
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
    std::string getDescription() const override { return "Calculate matrix determinant"; }
    std::vector<std::string> getPrompts() const override { 
        return { "Enter matrix dimension (n for nxn matrix): " }; 
    }
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
    std::string getDescription() const override { return "Calculate vector magnitude (norm)"; }
    std::vector<std::string> getPrompts() const override { 
        return { "How many dimensions? Enter count: " }; 
    }
    int getRequiredArgs() const override { return -1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::VECTOR_OPS; }
};



class AdvancedDivision : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        if (std::abs(args[1]) < SystemGlobal::EPSILON) 
            throw std::domain_error("Division by zero detected");
        return args[0] / args[1];
    }
    std::string getID() const override { return "MATH_DIVISION"; }
    std::string getDescription() const override { return "Safe division with zero-guard"; }
    std::vector<std::string> getPrompts() const override { 
        return { "Enter dividend: ", "Enter divisor: " }; 
    }
    int getRequiredArgs() const override { return 2; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};

class PowerFunction : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        return std::pow(args[0], args[1]);
    }
    std::string getID() const override { return "MATH_POWER"; }
    std::string getDescription() const override { return "Calculate x^y"; }
    std::vector<std::string> getPrompts() const override { 
        return { "Enter base: ", "Enter exponent: " }; 
    }
    int getRequiredArgs() const override { return 2; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};

class SquareRoot : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        if (args[0] < 0) throw std::domain_error("Cannot take square root of negative");
        return std::sqrt(args[0]);
    }
    std::string getID() const override { return "MATH_SQRT"; }
    std::string getDescription() const override { return "Calculate square root"; }
    std::vector<std::string> getPrompts() const override { 
        return { "Enter value: " }; 
    }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};

class NaturalLogarithm : public ICommand {
public:
    double execute(const std::vector<double>& args) override {
        if (args[0] <= 0) throw std::domain_error("Logarithm undefined for non-positive");
        return std::log(args[0]);
    }
    std::string getID() const override { return "MATH_LN"; }
    std::string getDescription() const override { return "Calculate natural logarithm"; }
    std::vector<std::string> getPrompts() const override { 
        return { "Enter value: " }; 
    }
    int getRequiredArgs() const override { return 1; }
    SystemGlobal::Domain getDomain() const override { return SystemGlobal::Domain::ADVANCED_MATH; }
};


class IntegrityChecker {
public:
    static bool runPreFlightChecks() {
        SystemLogger::write("Starting system integrity checks", SystemGlobal::LogLevel::INFO);

        try {
            TrigSine sine;
            double sinZero = sine.execute({0.0});
            if (std::abs(sinZero) > 1e-5) 
                throw std::runtime_error("Sine(0) != 0");
            SystemLogger::write("Trigonometry module verified", SystemGlobal::LogLevel::INFO);

            Statistics::Dataset testData;
            testData.addObservation(1.0);
            testData.addObservation(2.0);
            testData.addObservation(3.0);
            double mean = testData.calculateMean();
            if (std::abs(mean - 2.0) > 1e-5)
                throw std::runtime_error("Statistics mean calculation failed");
            SystemLogger::write("Statistics module verified", SystemGlobal::LogLevel::INFO);

            MatrixCalculus::MatrixEngine::Matrix m = {{1, 0}, {0, 1}};
            double det = MatrixCalculus::MatrixEngine::computeDeterminant(m);
            if (std::abs(det - 1.0) > 1e-5)
                throw std::runtime_error("Matrix determinant calculation failed");
            SystemLogger::write("Linear algebra module verified", SystemGlobal::LogLevel::INFO);

            VectorAlgebra::VectorND v({3.0, 4.0});
            double norm = v.magnitude();
            if (std::abs(norm - 5.0) > 1e-5)
                throw std::runtime_error("Vector norm calculation failed");
            SystemLogger::write("Vector operations verified", SystemGlobal::LogLevel::INFO);

            SystemLogger::write("=== ALL SYSTEMS NOMINAL ===", SystemGlobal::LogLevel::INFO);
            return true;
        }
        catch (const std::exception& e) {
            SystemLogger::write(std::string("CRITICAL: ") + e.what(), SystemGlobal::LogLevel::CRITICAL);
            return false;
        }
    }
};


class ApplicationController {
private:
    std::unique_ptr<AdaptiveBrain> brain;
    std::vector<std::shared_ptr<ICommand>> opRegistry;

public:
    ApplicationController() : brain(std::make_unique<AdaptiveBrain>()) {
        registerCommands();
        SystemLogger::initialize();
    }

    void launch() {
        if (!IntegrityChecker::runPreFlightChecks()) {
            std::cerr << "System self-test failed. Exiting.\n";
            return;
        }

        mainLoop();
    }

private:
    void registerCommands() {
        opRegistry.push_back(std::make_shared<StatsCalculateMean>());
        opRegistry.push_back(std::make_shared<StatsCalculateVariance>());
        opRegistry.push_back(std::make_shared<StatsCalculateStdDev>());
        opRegistry.push_back(std::make_shared<StatsCalculateMedian>());
        opRegistry.push_back(std::make_shared<StatsDetectOutliers>());

        opRegistry.push_back(std::make_shared<TrigSine>());
        opRegistry.push_back(std::make_shared<TrigCosine>());
        opRegistry.push_back(std::make_shared<TrigTangent>());

        opRegistry.push_back(std::make_shared<LinearAlgebraDeterminant>());
        opRegistry.push_back(std::make_shared<VectorNorm>());

        opRegistry.push_back(std::make_shared<AdvancedDivision>());
        opRegistry.push_back(std::make_shared<PowerFunction>());
        opRegistry.push_back(std::make_shared<SquareRoot>());
        opRegistry.push_back(std::make_shared<NaturalLogarithm>());
    }

    void mainLoop() {
        while (true) {
            displayMainUI();
            int input = getUserSelection();

            if (input == 0) {
                std::cout << brain->getStatistics() << "\n";
                std::cout << "Shutting down SOVEREIGN ENGINE...\n";
                break;
            }

            if (input > 0 && input <= (int)opRegistry.size()) {
                executeCommand(opRegistry[input - 1]);
            } else {
                std::cout << "Invalid selection. Try again.\n";
            }

            pauseForUser();
        }
    }

    void displayMainUI() {
        system("clear || cls");
        std::cout << std::string(70, '=') << "\n";
        std::cout << "     SOVEREIGN ADAPTIVE COMPUTATION ENGINE v6.2\n";
        std::cout << "     Professional Scientific Computing Platform\n";
        std::cout << std::string(70, '=') << "\n\n";

        int idx = 1;

        std::cout << "[STATISTICS OPERATIONS]\n";
        for (const auto& cmd : opRegistry) {
            if (cmd->getDomain() == SystemGlobal::Domain::STATISTICS) {
                std::cout << "  [" << idx++ << "] " << std::setw(20) << std::left 
                         << cmd->getID() << " - " << cmd->getDescription() << "\n";
            }
        }

        std::cout << "\n[TRIGONOMETRY]\n";
        for (const auto& cmd : opRegistry) {
            if (cmd->getDomain() == SystemGlobal::Domain::TRIGONOMETRY) {
                std::cout << "  [" << idx++ << "] " << std::setw(20) << std::left 
                         << cmd->getID() << " - " << cmd->getDescription() << "\n";
            }
        }

        std::cout << "\n[LINEAR ALGEBRA & VECTORS]\n";
        for (const auto& cmd : opRegistry) {
            if (cmd->getDomain() == SystemGlobal::Domain::LINEAR_ALGEBRA ||
                cmd->getDomain() == SystemGlobal::Domain::VECTOR_OPS) {
                std::cout << "  [" << idx++ << "] " << std::setw(20) << std::left 
                         << cmd->getID() << " - " << cmd->getDescription() << "\n";
            }
        }

        std::cout << "\n[ADVANCED MATH]\n";
        for (const auto& cmd : opRegistry) {
            if (cmd->getDomain() == SystemGlobal::Domain::ADVANCED_MATH) {
                std::cout << "  [" << idx++ << "] " << std::setw(20) << std::left 
                         << cmd->getID() << " - " << cmd->getDescription() << "\n";
            }
        }

        std::cout << "\n  [0] EXIT SYSTEM (view statistics)\n";
        std::cout << std::string(70, '=') << "\n";
    }

    void executeCommand(std::shared_ptr<ICommand> cmd) {
        try {
            brain->provideSuggestion(cmd->getID());

            std::vector<double> params;
            if (cmd->getRequiredArgs() == -1) {
                int count = (int)requestDouble(cmd->getPrompts()[0]);
                for (int i = 0; i < count; ++i) {
                    std::cout << "Value " << (i + 1) << ": ";
                    params.push_back(requestDouble(""));
                }
            } else {
                for (const auto& prompt : cmd->getPrompts()) {
                    params.push_back(requestDouble(prompt));
                }
            }

            double result = cmd->execute(params);

            std::cout << "\n" << std::string(60, '=') << "\n";
            std::cout << "OPERATION SUCCESSFUL\n";
            std::cout << "RESULT: " << std::fixed << std::setprecision(12) << result << "\n";
            std::cout << std::string(60, '=') << "\n";

            brain->recordSuccess(cmd->getID());
            SystemLogger::write("Operation " + cmd->getID() + " completed successfully", 
                              SystemGlobal::LogLevel::INFO);
        }
        catch (const std::exception& e) {
            std::cout << "\n" << std::string(60, '=') << "\n";
            std::cout << "OPERATION FAILED\n";
            std::cout << "ERROR: " << e.what() << "\n";
            std::cout << std::string(60, '=') << "\n";

            brain->recordFailure(cmd->getID(), e.what());
            SystemLogger::write(std::string("Operation failed: ") + e.what(), 
                              SystemGlobal::LogLevel::ERROR);
        }
    }

    double requestDouble(const std::string& prompt) {
        if (!prompt.empty()) std::cout << prompt;

        double value;
        while (!(std::cin >> value)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Enter a number: ";
        }
        std::cin.ignore(10000, '\n');
        return value;
    }

    int getUserSelection() {
        int selection;
        std::cout << "Selection: ";
        while (!(std::cin >> selection)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Enter selection: ";
        }
        std::cin.ignore(10000, '\n');
        return selection;
    }

    void pauseForUser() {
        std::cout << "\nPress ENTER to continue...";
        std::cin.get();
    }
};
