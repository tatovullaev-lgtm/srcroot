/** @file   RLDeconvolutor.h
    @class  RLDeconvolutor
    @author Nikolay Karpushkin (nkarpushkin@mail.ru)
    @brief  Class to apply Richardson-Lucy deconvolution
*/

#ifndef RLDeconvolutor_H
#define RLDeconvolutor_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric> 
#include <cassert>

#include "FairLogger.h"

class RLDeconvolutor
{
  public:
    RLDeconvolutor(const std::vector<float>& y, const std::vector<float>& kernel_, const float R2threshold_, const int max_iterations_)
    : blurred_signal(y),
      kernel(kernel_),
      reversed_kernel(kernel_),
      R2(),
      R2threshold(R2threshold_),
      max_iterations(max_iterations_),
      converged(false)
    {
        deconvolved.resize(y.size(), 1.0);
        std::reverse(reversed_kernel.begin(), reversed_kernel.end());
    }

    void setR2threshold(float thr) { R2threshold = thr; }
    void setMaxIterations(int max) { max_iterations = max; }

    bool runDeconvolution() {
        R2.clear();
        auto next_step = convolution(deconvolved, kernel);
        for (int iter = 0; iter < max_iterations; ++iter) {
            // constrain operator is needed to make the deconvolution procedure more stable. Excludes overfit to noise
            LOG(debug4) << "RLDeconvolutor : calculating error " << iter;
            std::vector<float> error = constrainOperator(elementwiseDivision(blurred_signal, next_step));
            LOG(debug4) << "RLDeconvolutor : recalculating deconvolved " << iter;
            deconvolved = elementwiseMultiplication(deconvolved, convolution(error, reversed_kernel));
            next_step = convolution(deconvolved, kernel);

            R2.push_back(R2score(blurred_signal, next_step));
            if (R2.back() > R2default)
                R2.back() = R2default;
            LOG(debug4) << "RLDeconvolutor : estimated error R2 " << R2.back();

            //converged
            if (R2.back() < R2threshold) {
                LOG(debug4) << "RLDeconvolutor : coverged at iter " << iter;
                converged = true;
                break;
            }
        }

        if(!converged)
            LOG(debug4) << "RLDeconvolutor : did not converge.";

        // Find minima between maxima
        LOG(debug4) << "RLDeconvolutor : findCamelPoints";
        camel_points = findCamelPoints();

        // Extract signal positions and amplitudes
        LOG(debug4) << "RLDeconvolutor : extracting";
        extractSignalInfo();

        return converged;
    }

    // Individual signals -- pairs of position and amplitude
    const std::vector<std::pair<size_t, float>>& getSignalInfo() const {
        return signal_info;
    }

    // Individual signals -- deconvolved ~ delta-functions
    const std::vector<std::vector<float>>& getSignalDeconvolution() const {
        return signal_deconvolved;
    }

    // Individual signals -- reconvolved
    const std::vector<std::vector<float>>& getSignalDecomposition() const {
        return signal_decomposed;
    }

    std::vector<float> getWfmApproximation() const {
        return convolution(deconvolved, kernel);
    }

    const std::vector<float>& getR2() const {
        return R2;
    }

private:
    std::vector<float> blurred_signal;   // original data with possible pileup etc. Tested to work well with positive data with baseline at zero
    std::vector<float> deconvolved;      // deconvolved (delta-functions)
    std::vector<float> kernel;           // kernel of convolution (PSF or Ethalon signal)
    std::vector<float> reversed_kernel;
    std::vector<float> R2;
    float R2threshold;
    inline static constexpr float R2default = 2.0;
    int max_iterations;
    bool converged;

    std::vector<size_t> camel_points; // in case of pileup it is the minimum between two succeding maxima
    std::vector<std::pair<size_t, float>> signal_info; // signal positions and amplitudes
    std::vector<std::vector<float>> signal_deconvolved; // initial waveform is deconvolved
    std::vector<std::vector<float>> signal_decomposed; // initial waveform is decomposed into vector of signals

    std::vector<float> convolution(const std::vector<float>& signal, const std::vector<float>& kern) const {
        // convolution like numpy.convolve with 'same' option.
        // TODO think about FFT for large-time signals.
        size_t signal_size = signal.size();
        size_t kernel_size = kern.size();
        size_t padding = kernel_size / 2;  // Padding for both sides

        std::vector<float> padded_signal(signal_size + 2 * padding, 0.0);
        std::copy(signal.begin(), signal.end(), padded_signal.begin() + padding);

        std::vector<float> result(signal_size, 0.0);

        for (size_t i = 0; i < signal_size; ++i)
            for (size_t j = 0; j < kernel_size; ++j)
                result.at(i) += padded_signal.at(i + kernel_size - 1 - j) * kern.at(j);

        return result;
    }

    std::vector<float> constrainOperator(const std::vector<float>& data_array) {
        std::vector<float> out_array(data_array.size(), 0.0);

        // Lambda function to define the condition (positivity)
        auto condition = [](float number) {
            return number > 0.0;
        };

        // Apply the condition
        for (size_t i = 0; i < data_array.size(); ++i) {
            if (condition(data_array.at(i)))
                out_array.at(i) = data_array.at(i);
            else
                out_array.at(i) = 0.0;
        }
        return out_array;
    }

    std::vector<float> elementwiseMultiplication(const std::vector<float>& vec1, const std::vector<float>& vec2) const {
        std::vector<float> result(vec1.size());
        assert(vec1.size() == vec2.size());

        for (size_t i = 0; i < vec1.size(); ++i)
            result.at(i) = vec1.at(i) * vec2.at(i);

        return result;
    }

    std::vector<float> elementwiseDivision(const std::vector<float>& vec1, const std::vector<float>& vec2) const {
        std::vector<float> result(vec1.size());
        assert(vec1.size() == vec2.size());

        for (size_t i = 0; i < vec1.size(); ++i)
            result.at(i) = vec1.at(i) / (vec2.at(i) + std::numeric_limits<float>::min());

        return result;
    }

    float R2score(const std::vector<float>& true_values, const std::vector<float>& predicted_values) const {
        assert(true_values.size() == predicted_values.size());
        if (true_values.empty()) return R2default;

        float mean_true = std::accumulate(true_values.begin(), true_values.end(), 0.0) / true_values.size();
        float ss_total = 0.0;
        float ss_residual = 0.0;

        for (size_t i = 0; i < true_values.size(); ++i) {
            ss_total += std::pow(true_values.at(i) - mean_true, 2);
            ss_residual += std::pow(true_values.at(i) - predicted_values.at(i), 2);
        }

        return ss_residual / (ss_total + std::numeric_limits<float>::min());
    }

    std::vector<size_t> findMaximaIndices(const std::vector<float>& vec) const {
        std::vector<size_t> maxima_indices;
        if (vec.empty()) return maxima_indices;

        for (size_t i = 1; i < vec.size() - 1; ++i) {
            if (vec.at(i) > vec.at(i - 1) && vec.at(i) > vec.at(i + 1)) {
                maxima_indices.push_back(i);
            }
        }

        return maxima_indices;
    }

    std::vector<size_t> findCamelPoints() const {
        std::vector<size_t> maxima_indices = findMaximaIndices(deconvolved);
        std::vector<size_t> c_points;

        LOG(debug4) << "RLDeconvolutor : findCamelPoints between total maxima " << maxima_indices.size();
        if (!maxima_indices.empty()) {
            for (size_t i = 0; i < maxima_indices.size() - 1; ++i) {
                // Find the minimum value between each pair of maxima
                auto camel_area = getSubvector(deconvolved, maxima_indices.at(i), maxima_indices.at(i + 1));
                if (!camel_area.empty()) {
                    size_t camel_point = maxima_indices.at(i) + std::distance(camel_area.begin(), std::min_element(camel_area.begin(), camel_area.end()));
                    c_points.push_back(camel_point);
                }
            }
        }
        c_points.insert(c_points.begin(), 0);
        c_points.push_back(deconvolved.size() - 1);
        for(auto it : c_points)
            LOG(debug4) << "RLDeconvolutor : found CamelPoints " << it;

        return c_points;
    }

    std::vector<float> getSubvector(const std::vector<float>& vec, size_t start, size_t end) const {
        return std::vector<float>(vec.begin() + start, vec.begin() + end);
    }

    void extractSignalInfo() {
        signal_info.clear();
        if (camel_points.empty()) return;

        // Process regions between camel points
        for (size_t i = 0; i < camel_points.size() - 1; ++i) {
            int start_index = camel_points.at(i);
            int end_index = camel_points.at(i + 1);

            // Get the subvector corresponding to the region
            std::vector<float> subvector = getSubvector(deconvolved, start_index, end_index);
            std::vector<float> thisVector(deconvolved.size(), 0.0);
            std::copy(subvector.begin(), subvector.end(), thisVector.begin() + start_index);
            signal_deconvolved.push_back(thisVector);

            // Convolve the subvector with the kernel to obtain the reconstructed signal
            std::vector<float> reconstructed_signal = convolution(thisVector, kernel);
            signal_decomposed.push_back(reconstructed_signal);

            // Find the maximum amplitude and its position in the reconstructed signal
            auto max_amplitude_iter = std::max_element(reconstructed_signal.begin(), reconstructed_signal.end());
            float max_amplitude = *max_amplitude_iter;
            int max_amplitude_position = std::distance(reconstructed_signal.begin(), max_amplitude_iter);

            // Store the result in the signal_info vector
            signal_info.push_back(std::make_pair(max_amplitude_position, max_amplitude));
        }
    }

};

#endif
