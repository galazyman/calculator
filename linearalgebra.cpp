#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <limits>
#include <map>
#include <stdexcept>

namespace Statistics {
    class Dataset {
    private:
        std::vector<double> observations;
        mutable std::vector<double> sortedCache;
        mutable bool cacheValid = false;
        mutable std::map<int, double> percentileCache;

        void ensureSorted() const {
            if (!cacheValid) {
                sortedCache = observations;
                std::sort(sortedCache.begin(), sortedCache.end());
                cacheValid = true;
                percentileCache.clear();
            }
        }

        void invalidateCache() {
            cacheValid = false;
            percentileCache.clear();
        }

    public:
        Dataset() = default;
        Dataset(const Dataset&) = default;
        Dataset& operator=(const Dataset&) = default;
        Dataset(Dataset&&) noexcept = default;
        Dataset& operator=(Dataset&&) noexcept = default;
        ~Dataset() noexcept = default;

        void addObservation(double val) {
            observations.push_back(val);
            invalidateCache();
        }

        void addObservations(const std::vector<double>& vals) {
            observations.insert(observations.end(), vals.begin(), vals.end());
            invalidateCache();
        }

        void reserve(size_t capacity) {
            observations.reserve(capacity);
        }

        size_t size() const { return observations.size(); }
        void clear() {
            observations.clear();
            invalidateCache();
        }

        const std::vector<double>& getRawData() const { return observations; }

        double calculateMean() const {
            if (observations.empty())
                throw std::runtime_error("Cannot calculate mean of empty dataset.");
            return std::accumulate(observations.begin(), observations.end(), 0.0)
                / observations.size();
        }

        double calculateGeometricMean() const {
            if (observations.empty())
                throw std::runtime_error("Cannot calculate geometric mean of empty dataset.");

            double product = 1.0;
            for (double val : observations) {
                if (val <= 0) throw std::runtime_error("Geometric mean requires positive values.");
                product *= val;
            }
            return std::pow(product, 1.0 / observations.size());
        }

        double calculateHarmonicMean() const {
            if (observations.empty())
                throw std::runtime_error("Cannot calculate harmonic mean of empty dataset.");

            double reciprocalSum = 0.0;
            for (double val : observations) {
                if (val == 0) throw std::runtime_error("Harmonic mean undefined for zero values.");
                reciprocalSum += 1.0 / val;
            }
            return observations.size() / reciprocalSum;
        }

        double getMedian() const {
            if (observations.empty()) return 0.0;

            ensureSorted();
            size_t n = sortedCache.size();

            if (n % 2 == 0) {
                return (sortedCache[n / 2 - 1] + sortedCache[n / 2]) / 2.0;
            }
            return sortedCache[n / 2];
        }

        double getMode() const {
            if (observations.empty())
                throw std::runtime_error("Cannot calculate mode of empty dataset.");

            std::map<double, int> frequency;
            for (double val : observations) frequency[val]++;

            double mode = observations[0];
            int maxFreq = 0;
            for (const auto& [val, freq] : frequency) {
                if (freq > maxFreq) {
                    maxFreq = freq;
                    mode = val;
                }
            }
            return mode;
        }

        double calculateVariance() const {
            if (observations.empty())
                throw std::runtime_error("Cannot calculate variance of empty dataset.");

            double mean = calculateMean();
            double sumSquaredDiff = std::accumulate(
                observations.begin(), observations.end(), 0.0,
                [mean](double acc, double val) {
                    double diff = val - mean;
                    return acc + diff * diff;
                }
            );
            return sumSquaredDiff / observations.size();
        }

        double calculateSampleVariance() const {
            if (observations.size() < 2)
                throw std::runtime_error("Sample variance requires at least 2 observations.");

            double mean = calculateMean();
            double sumSquaredDiff = std::accumulate(
                observations.begin(), observations.end(), 0.0,
                [mean](double acc, double val) {
                    double diff = val - mean;
                    return acc + diff * diff;
                }
            );
            return sumSquaredDiff / (observations.size() - 1);
        }

        double calculateStdDev() const {
            return std::sqrt(calculateVariance());
        }

        double calculateSampleStdDev() const {
            return std::sqrt(calculateSampleVariance());
        }

        double getRange() const {
            if (observations.empty())
                throw std::runtime_error("Cannot calculate range of empty dataset.");

            auto [minIt, maxIt] = std::minmax_element(observations.begin(), observations.end());
            return *maxIt - *minIt;
        }

        double getCoefficientOfVariation() const {
            double mean = calculateMean();
            if (std::abs(mean) < 1e-15)
                throw std::runtime_error("Coefficient of variation undefined for near-zero mean.");
            return calculateStdDev() / std::abs(mean);
        }

        double getPercentile(int p) const {
            if (observations.empty())
                throw std::runtime_error("Cannot calculate percentile of empty dataset.");
            if (p < 0 || p > 100)
                throw std::invalid_argument("Percentile must be between 0 and 100.");

            if (percentileCache.count(p))
                return percentileCache[p];

            ensureSorted();
            size_t n = sortedCache.size();

            if (p == 0) return sortedCache[0];
            if (p == 100) return sortedCache[n - 1];

            double position = (p / 100.0) * (n - 1);
            size_t lower = static_cast<size_t>(position);
            double fraction = position - lower;

            double result = sortedCache[lower] * (1.0 - fraction) + sortedCache[lower + 1] * fraction;
            percentileCache[p] = result;
            return result;
        }

        double getQ1() const { return getPercentile(25); }
        double getQ2() const { return getPercentile(50); }
        double getQ3() const { return getPercentile(75); }

        double getInterquartileRange() const {
            return getQ3() - getQ1();
        }

        double getSkewness() const {
            if (observations.size() < 3)
                throw std::runtime_error("Skewness requires at least 3 observations.");

            double mean = calculateMean();
            double stdDev = calculateStdDev();
            if (stdDev < 1e-15)
                throw std::runtime_error("Skewness undefined for zero standard deviation.");

            double m3 = 0.0;
            for (double val : observations) {
                double z = (val - mean) / stdDev;
                m3 += z * z * z;
            }
            return m3 / observations.size();
        }

        double getKurtosis() const {
            if (observations.size() < 4)
                throw std::runtime_error("Kurtosis requires at least 4 observations.");

            double mean = calculateMean();
            double stdDev = calculateStdDev();
            if (stdDev < 1e-15)
                throw std::runtime_error("Kurtosis undefined for zero standard deviation.");

            double m4 = 0.0;
            for (double val : observations) {
                double z = (val - mean) / stdDev;
                m4 += z * z * z * z;
            }
            return (m4 / observations.size()) - 3.0;
        }

        std::vector<double> detectOutliersIQR(double multiplier = 1.5) const {
            double q1 = getQ1();
            double q3 = getQ3();
            double iqr = q3 - q1;

            double lowerBound = q1 - multiplier * iqr;
            double upperBound = q3 + multiplier * iqr;

            std::vector<double> outliers;
            for (double val : observations) {
                if (val < lowerBound || val > upperBound)
                    outliers.push_back(val);
            }
            return outliers;
        }

        std::vector<double> detectOutliersZScore(double threshold = 3.0) const {
            double mean = calculateMean();
            double stdDev = calculateStdDev();
            if (stdDev < 1e-15)
                throw std::runtime_error("Z-score method undefined for zero standard deviation.");

            std::vector<double> outliers;
            for (double val : observations) {
                double zScore = std::abs((val - mean) / stdDev);
                if (zScore > threshold)
                    outliers.push_back(val);
            }
            return outliers;
        }

        std::vector<double> normalizeMinMax() const {
            if (observations.empty())
                throw std::runtime_error("Cannot normalize empty dataset.");

            auto [minIt, maxIt] = std::minmax_element(observations.begin(), observations.end());
            double minVal = *minIt;
            double maxVal = *maxIt;
            double range = maxVal - minVal;

            if (range < 1e-15)
                throw std::runtime_error("Cannot normalize dataset with zero range.");

            std::vector<double> normalized;
            for (double val : observations) {
                normalized.push_back((val - minVal) / range);
            }
            return normalized;
        }

        std::vector<double> standardize() const {
            double mean = calculateMean();
            double stdDev = calculateStdDev();
            if (stdDev < 1e-15)
                throw std::runtime_error("Cannot standardize with zero standard deviation.");

            std::vector<double> standardized;
            for (double val : observations) {
                standardized.push_back((val - mean) / stdDev);
            }
            return standardized;
        }

        std::vector<double> logTransform(double offset = 1.0) const {
            std::vector<double> transformed;
            for (double val : observations) {
                transformed.push_back(std::log(val + offset));
            }
            return transformed;
        }

        double calculateCovariance(const Dataset& other) const {
            if (observations.size() != other.observations.size())
                throw std::invalid_argument("Datasets must have equal size for covariance.");
            if (observations.size() < 2)
                throw std::runtime_error("Covariance requires at least 2 observations.");

            double mean1 = calculateMean();
            double mean2 = other.calculateMean();

            double sumProduct = 0.0;
            for (size_t i = 0; i < observations.size(); ++i) {
                sumProduct += (observations[i] - mean1) * (other.observations[i] - mean2);
            }
            return sumProduct / (observations.size() - 1);
        }

        double calculatePearsonCorrelation(const Dataset& other) const {
            if (observations.size() != other.observations.size())
                throw std::invalid_argument("Datasets must have equal size for correlation.");

            double covariance = calculateCovariance(other);
            double stdDev1 = calculateSampleStdDev();
            double stdDev2 = other.calculateSampleStdDev();

            if (stdDev1 < 1e-15 || stdDev2 < 1e-15)
                throw std::runtime_error("Correlation undefined for zero standard deviation.");

            return covariance / (stdDev1 * stdDev2);
        }

        struct SummaryStatistics {
            double count = 0, minimum = 0, q1 = 0, median = 0, q3 = 0, maximum = 0;
            double mean = 0, stdDev = 0, variance = 0, skewness = 0, kurtosis = 0;
            double iqr = 0, range = 0, coeffVar = 0;
        };

        SummaryStatistics getSummary() const {
            SummaryStatistics stats;
            stats.count = observations.size();
            stats.minimum = *std::min_element(observations.begin(), observations.end());
            stats.maximum = *std::max_element(observations.begin(), observations.end());
            stats.q1 = getQ1();
            stats.median = getMedian();
            stats.q3 = getQ3();
            stats.mean = calculateMean();
            stats.stdDev = calculateStdDev();
            stats.variance = calculateVariance();
            stats.skewness = getSkewness();
            stats.kurtosis = getKurtosis();
            stats.iqr = getInterquartileRange();
            stats.range = getRange();
            stats.coeffVar = getCoefficientOfVariation();
            return stats;
        }

        std::vector<double> movingAverage(size_t windowSize) const {
            if (windowSize > observations.size() || windowSize == 0)
                throw std::invalid_argument("Invalid window size for moving average.");

            std::vector<double> result;
            for (size_t i = 0; i <= observations.size() - windowSize; ++i) {
                double sum = std::accumulate(
                    observations.begin() + i,
                    observations.begin() + i + windowSize,
                    0.0
                );
                result.push_back(sum / windowSize);
            }
            return result;
        }

        void removeOutliers(const std::vector<double>& outliersToRemove) {
            for (double outlier : outliersToRemove) {
                auto it = std::find(observations.begin(), observations.end(), outlier);
                if (it != observations.end()) {
                    observations.erase(it);
                }
            }
            invalidateCache();
        }
    };
}

namespace VectorAlgebra {
    struct Vector3D {
        double x, y, z;

        Vector3D() : x(0), y(0), z(0) {}
        Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

        double magnitude() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        Vector3D normalized() const {
            double mag = magnitude();
            if (mag < 1e-15) throw std::runtime_error("Cannot normalize zero vector.");
            return Vector3D(x / mag, y / mag, z / mag);
        }

        double dot(const Vector3D& other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        Vector3D cross(const Vector3D& other) const {
            return Vector3D(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        double distanceTo(const Vector3D& other) const {
            double dx = x - other.x;
            double dy = y - other.y;
            double dz = z - other.z;
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }
    };

    class VectorND {
    private:
        std::vector<double> components;

    public:
        VectorND() = default;
        VectorND(size_t size) : components(size, 0.0) {}
        VectorND(const std::vector<double>& data) : components(data) {}

        size_t size() const { return components.size(); }
        double& operator[](size_t i) { return components[i]; }
        const double& operator[](size_t i) const { return components[i]; }

        double magnitude() const {
            return std::sqrt(std::accumulate(components.begin(), components.end(), 0.0,
                [](double acc, double val) { return acc + val * val; }));
        }

        VectorND normalized() const {
            double mag = magnitude();
            if (mag < 1e-15) throw std::runtime_error("Cannot normalize zero vector.");
            VectorND result(components.size());
            for (size_t i = 0; i < components.size(); ++i) {
                result[i] = components[i] / mag;
            }
            return result;
        }

        double dot(const VectorND& other) const {
            if (components.size() != other.components.size())
                throw std::invalid_argument("Vectors must have same dimension.");
            double result = 0.0;
            for (size_t i = 0; i < components.size(); ++i) {
                result += components[i] * other.components[i];
            }
            return result;
        }

        double distanceTo(const VectorND& other) const {
            if (components.size() != other.components.size())
                throw std::invalid_argument("Vectors must have same dimension.");
            double sumSquares = 0.0;
            for (size_t i = 0; i < components.size(); ++i) {
                double diff = components[i] - other.components[i];
                sumSquares += diff * diff;
            }
            return std::sqrt(sumSquares);
        }
    };
}

namespace MatrixCalculus {
    constexpr double MATRIX_EPSILON = 1e-9;

    inline bool isNearZero(double value, double eps = MATRIX_EPSILON) noexcept {
        return std::abs(value) < eps;
    }

    class MatrixEngine {
    public:
        using Matrix = std::vector<std::vector<double>>;

        static constexpr double getMachineEpsilon() noexcept {
            return std::numeric_limits<double>::epsilon();
        }

        static void validateSquare(const Matrix& m) {
            if (m.empty()) return;
            size_t n = m.size();
            for (const auto& row : m) {
                if (row.size() != n)
                    throw std::invalid_argument("Matrix must be square.");
            }
        }

        static void validateMultiplication(const Matrix& a, const Matrix& b) {
            if (a.empty() || b.empty() || a[0].size() != b.size())
                throw std::invalid_argument("Incompatible dimensions for matrix multiplication.");
        }

        static std::pair<size_t, size_t> getDimensions(const Matrix& m) {
            if (m.empty()) return { 0, 0 };
            return { m.size(), m[0].size() };
        }

        static double computeTrace(const Matrix& m) {
            validateSquare(m);
            double trace = 0.0;
            for (size_t i = 0; i < m.size(); ++i) {
                trace += m[i][i];
            }
            return trace;
        }

        static double computeFrobeniusNorm(const Matrix& m) {
            double sumSquares = 0.0;
            for (const auto& row : m) {
                for (double val : row) {
                    sumSquares += val * val;
                }
            }
            return std::sqrt(sumSquares);
        }

        static double computeConditionNumber(const Matrix& m) {
            double norm = computeFrobeniusNorm(m);
            Matrix inv = invertMatrix(m);
            double invNorm = computeFrobeniusNorm(inv);
            return norm * invNorm;
        }

        static Matrix add(const Matrix& a, const Matrix& b) {
            if (getDimensions(a) != getDimensions(b))
                throw std::invalid_argument("Matrices must have same dimensions.");

            Matrix result(a.size(), std::vector<double>(a[0].size()));
            for (size_t i = 0; i < a.size(); ++i) {
                for (size_t j = 0; j < a[i].size(); ++j) {
                    result[i][j] = a[i][j] + b[i][j];
                }
            }
            return result;
        }

        static Matrix subtract(const Matrix& a, const Matrix& b) {
            if (getDimensions(a) != getDimensions(b))
                throw std::invalid_argument("Matrices must have same dimensions.");

            Matrix result(a.size(), std::vector<double>(a[0].size()));
            for (size_t i = 0; i < a.size(); ++i) {
                for (size_t j = 0; j < a[i].size(); ++j) {
                    result[i][j] = a[i][j] - b[i][j];
                }
            }
            return result;
        }

        static Matrix scalarMultiply(double scalar, const Matrix& m) {
            Matrix result(m.size(), std::vector<double>(m[0].size()));
            for (size_t i = 0; i < m.size(); ++i) {
                for (size_t j = 0; j < m[i].size(); ++j) {
                    result[i][j] = scalar * m[i][j];
                }
            }
            return result;
        }

        static Matrix multiply(const Matrix& a, const Matrix& b) {
            validateMultiplication(a, b);

            size_t rows = a.size();
            size_t cols = b[0].size();
            size_t inner = a[0].size();

            Matrix result(rows, std::vector<double>(cols, 0.0));
            for (size_t i = 0; i < rows; ++i) {
                for (size_t k = 0; k < inner; ++k) {
                    for (size_t j = 0; j < cols; ++j) {
                        result[i][j] += a[i][k] * b[k][j];
                    }
                }
            }
            return result;
        }

        static Matrix transpose(const Matrix& m) {
            if (m.empty()) return {};

            size_t rows = m.size();
            size_t cols = m[0].size();
            Matrix result(cols, std::vector<double>(rows));

            for (size_t i = 0; i < rows; ++i) {
                for (size_t j = 0; j < cols; ++j) {
                    result[j][i] = m[i][j];
                }
            }
            return result;
        }

        static Matrix elementWiseMultiply(const Matrix& a, const Matrix& b) {
            if (getDimensions(a) != getDimensions(b))
                throw std::invalid_argument("Matrices must have same dimensions.");

            Matrix result(a.size(), std::vector<double>(a[0].size()));
            for (size_t i = 0; i < a.size(); ++i) {
                for (size_t j = 0; j < a[i].size(); ++j) {
                    result[i][j] = a[i][j] * b[i][j];
                }
            }
            return result;
        }

        static double computeDeterminant(Matrix matrix) {
            validateSquare(matrix);
            const size_t n = matrix.size();

            if (n == 0) return 1.0;
            if (n == 1) return matrix[0][0];

            double det = 1.0;

            for (size_t i = 0; i < n; ++i) {
                size_t pivot = i;
                double maxAbsValue = std::abs(matrix[i][i]);
                for (size_t j = i + 1; j < n; ++j) {
                    double absValue = std::abs(matrix[j][i]);
                    if (absValue > maxAbsValue) {
                        maxAbsValue = absValue;
                        pivot = j;
                    }
                }

                if (pivot != i) {
                    std::swap(matrix[i], matrix[pivot]);
                    det *= -1.0;
                }

                if (isNearZero(matrix[i][i]))
                    return 0.0;

                det *= matrix[i][i];

                const double pivot_element = matrix[i][i];
                for (size_t j = i + 1; j < n; ++j)
                    matrix[i][j] /= pivot_element;

                for (size_t j = i + 1; j < n; ++j) {
                    const double factor = matrix[j][i];
                    if (!isNearZero(factor)) {
                        for (size_t k = i + 1; k < n; ++k)
                            matrix[j][k] -= factor * matrix[i][k];
                    }
                }
            }

            return det;
        }

        static bool isInvertible(const Matrix& m) noexcept {
            try {
                double det = computeDeterminant(m);
                return !isNearZero(det);
            }
            catch (const std::exception&) {
                return false;
            }
        }

        static Matrix invertMatrix(Matrix m) {
            validateSquare(m);
            size_t n = m.size();

            if (!isInvertible(m))
                throw std::runtime_error("Matrix is singular and cannot be inverted.");

            Matrix inv(n, std::vector<double>(n, 0.0));
            for (size_t i = 0; i < n; ++i) inv[i][i] = 1.0;

            for (size_t i = 0; i < n; ++i) {
                size_t pivot = i;
                for (size_t j = i + 1; j < n; ++j) {
                    if (std::abs(m[j][i]) > std::abs(m[pivot][i]))
                        pivot = j;
                }

                std::swap(m[i], m[pivot]);
                std::swap(inv[i], inv[pivot]);

                double pivot_elem = m[i][i];
                for (size_t j = 0; j < n; ++j) {
                    m[i][j] /= pivot_elem;
                    inv[i][j] /= pivot_elem;
                }

                for (size_t j = 0; j < n; ++j) {
                    if (j != i) {
                        double factor = m[j][i];
                        for (size_t k = 0; k < n; ++k) {
                            m[j][k] -= factor * m[i][k];
                            inv[j][k] -= factor * inv[i][k];
                        }
                    }
                }
            }

            return inv;
        }

        struct LUDecomposition {
            Matrix L, U;
            std::vector<size_t> permutation;
        };

        static LUDecomposition computeLU(Matrix m) {
            validateSquare(m);
            size_t n = m.size();

            Matrix L(n, std::vector<double>(n, 0.0));
            Matrix U(n, std::vector<double>(n, 0.0));
            std::vector<size_t> perm(n);
            for (size_t i = 0; i < n; ++i) perm[i] = i;

            for (size_t i = 0; i < n; ++i) {
                size_t pivot = i;
                for (size_t j = i + 1; j < n; ++j) {
                    if (std::abs(m[j][i]) > std::abs(m[pivot][i]))
                        pivot = j;
                }

                if (pivot != i) {
                    std::swap(m[i], m[pivot]);
                    std::swap(perm[i], perm[pivot]);
                }

                L[i][i] = 1.0;
                U[i][i] = m[i][i];

                if (!isNearZero(m[i][i])) {
                    for (size_t j = i + 1; j < n; ++j) {
                        L[j][i] = m[j][i] / m[i][i];
                        U[i][j] = m[i][j];
                    }

                    for (size_t j = i + 1; j < n; ++j) {
                        for (size_t k = i + 1; k < n; ++k) {
                            m[j][k] -= L[j][i] * U[i][k];
                        }
                    }
                }
            }

            return { L, U, perm };
        }

        static int computeRank(Matrix m) {
            if (m.empty()) return 0;

            size_t rows = m.size();
            size_t cols = m[0].size();
            int rank = 0;

            std::vector<bool> col_used(cols, false);

            for (size_t col = 0; col < cols && rank < rows; ++col) {
                size_t pivot_row = rows;
                for (size_t row = rank; row < rows; ++row) {
                    if (!isNearZero(m[row][col]) && (pivot_row == rows ||
                        std::abs(m[row][col]) > std::abs(m[pivot_row][col]))) {
                        pivot_row = row;
                    }
                }

                if (pivot_row == rows) continue;

                std::swap(m[rank], m[pivot_row]);
                col_used[col] = true;

                double pivot = m[rank][col];
                for (size_t j = col; j < cols; ++j) m[rank][j] /= pivot;

                for (size_t i = 0; i < rows; ++i) {
                    if (i != rank && !isNearZero(m[i][col])) {
                        double factor = m[i][col];
                        for (size_t j = col; j < cols; ++j) {
                            m[i][j] -= factor * m[rank][j];
                        }
                    }
                }

                rank++;
            }

            return rank;
        }

        static std::vector<double> solveLeastSquares(const Matrix& A, const std::vector<double>& b) {
            if (A.empty() || A.size() != b.size())
                throw std::invalid_argument("Dimension mismatch for least squares.");

            size_t m = A.size();
            size_t n = A[0].size();

            Matrix AT = transpose(A);
            Matrix ATA = multiply(AT, A);
            Matrix ATA_inv = invertMatrix(ATA);

            std::vector<double> ATb(n, 0.0);
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = 0; j < m; ++j) {
                    ATb[i] += AT[i][j] * b[j];
                }
            }

            std::vector<double> x(n, 0.0);
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = 0; j < n; ++j) {
                    x[i] += ATA_inv[i][j] * ATb[j];
                }
            }

            return x;
        }

        static std::vector<double> solveLinearSystem(Matrix A, std::vector<double> b) {
            validateSquare(A);
            if (A.size() != b.size())
                throw std::invalid_argument("System dimension mismatch.");

            size_t n = A.size();

            for (size_t i = 0; i < n; ++i) {
                size_t pivot = i;
                for (size_t j = i + 1; j < n; ++j) {
                    if (std::abs(A[j][i]) > std::abs(A[pivot][i]))
                        pivot = j;
                }

                std::swap(A[i], A[pivot]);
                std::swap(b[i], b[pivot]);

                if (isNearZero(A[i][i]))
                    throw std::runtime_error("Singular matrix in linear system solver.");

                for (size_t j = i + 1; j < n; ++j) {
                    double factor = A[j][i] / A[i][i];
                    for (size_t k = i; k < n; ++k) {
                        A[j][k] -= factor * A[i][k];
                    }
                    b[j] -= factor * b[i];
                }
            }

            std::vector<double> x(n);
            for (int i = n - 1; i >= 0; --i) {
                x[i] = b[i];
                for (size_t j = i + 1; j < n; ++j) {
                    x[i] -= A[i][j] * x[j];
                }
                x[i] /= A[i][i];
            }

            return x;
        }

        static double estimateLargestEigenvalue(const Matrix& m, int maxIterations = 100, double tol = 1e-6) {
            validateSquare(m);
            size_t n = m.size();

            std::vector<double> v(n, 1.0 / std::sqrt(n));

            for (int iter = 0; iter < maxIterations; ++iter) {
                std::vector<double> Av(n, 0.0);
                for (size_t i = 0; i < n; ++i) {
                    for (size_t j = 0; j < n; ++j) {
                        Av[i] += m[i][j] * v[j];
                    }
                }

                double norm = std::sqrt(std::accumulate(Av.begin(), Av.end(), 0.0,
                    [](double acc, double val) { return acc + val * val; }));

                if (norm < 1e-15) break;

                std::vector<double> v_new(n);
                for (size_t i = 0; i < n; ++i) v_new[i] = Av[i] / norm;

                double error = 0.0;
                for (size_t i = 0; i < n; ++i) {
                    error += std::abs(v_new[i] - v[i]);
                }
                if (error < tol) break;

                v = v_new;
            }

            double eigenvalue = 0.0;
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = 0; j < n; ++j) {
                    eigenvalue += m[i][j] * v[i] * v[j];
                }
            }

            return eigenvalue;
        }
    };
}