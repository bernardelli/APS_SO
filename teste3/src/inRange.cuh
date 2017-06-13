#pragma once
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudafilters.hpp>

void inRange_gpu(cv::cuda::GpuMat &src, cv::Scalar &lowerb, cv::Scalar &upperb, cv::cuda::GpuMat &dst);
