// This file is part of the xxxxxxxxx project.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

/*
* File:   uncertainty.h
* Author: xxxxxxxxx xxxxxxxxx
*
* Created on November 5, 2017, 11:30 AM
*/

#pragma once
#include <vector>
#include <iostream>
#include <memory>

#define _USE_MATH_DEFINES
#include <cmath>

#include "ceres/ceres.h"


namespace cov {

enum EAlgorithm {
	eAlgorithmSvdQrIteration = 0,
	eAlgorithmSvdDeviceAndconquer = 1,
	eAlgorithmTaylorExpansion = 2,
	eAlgorithmNullspaceBounding = 3,
	eAlgorithmLhuilier = 4
};


inline std::string EAlgorithm_enumToString(EAlgorithm alg) {
    switch (alg) {
        case eAlgorithmSvdQrIteration: return "SVD_QR_ITERATION";
        case eAlgorithmSvdDeviceAndconquer: return "SVD_DEVIDE_AND_CONQUER";
		case eAlgorithmLhuilier: return "LHUILLIER";
        case eAlgorithmTaylorExpansion: return "TE-INVERSION";
		case eAlgorithmNullspaceBounding: return "NBUP";
        default: return "not defined";
    }
}

//LHUILLIER, TE-INVERSION, NBUP

inline EAlgorithm EAlgorithm_stringToEnum(const std::string& algorithm) {
    if (algorithm == "SVD_QR_ITERATION")
        return eAlgorithmSvdQrIteration;
    if (algorithm == "SVD_DEVIDE_AND_CONQUER")
        return eAlgorithmSvdDeviceAndconquer;
	if (algorithm == "LHUILLIER")
		return eAlgorithmLhuilier;
    if (algorithm == "TE-INVERSION")
        return eAlgorithmTaylorExpansion;
	if (algorithm == "NBUP")
		return eAlgorithmNullspaceBounding;
    throw std::runtime_error(std::string("Unrecognized algorithm: ") + algorithm);
}

struct Options {
public:
    double _epsilon, _lambda;
    EAlgorithm _algorithm;
    int _numCams, _camParams, _numPoints, _numObs, _svdRemoveN, _maxIterTE;
    int *_pts2fix = NULL;
    bool _debug = false;
	bool _computePtsCov = false;

    Options() : _lambda(-1), _svdRemoveN(-1), _maxIterTE(-1) {}

    Options(int numCams, int camParams, int numPoints, int numObs) :
        _algorithm(eAlgorithmTaylorExpansion), _epsilon(1e-10), _lambda(-1), _numCams( numCams ), _camParams(camParams), _numPoints(numPoints), _numObs(numObs), _svdRemoveN(-1), _maxIterTE(-1) {}

    Options(EAlgorithm algorithm, double eps_or_lamb, int numCams, int camParams, int numPoints, int numObs) :
        _algorithm(algorithm), _epsilon(eps_or_lamb), _lambda(eps_or_lamb), _numCams( numCams ), _camParams(camParams), _numPoints(numPoints), _numObs(numObs), _svdRemoveN(-1), _maxIterTE(-1) {}

    ~Options() {
        free(_pts2fix);
    }
};

struct Statistic {
    double timeCreateJ, timeComposeH, timeScaleJH, timeFixJ, timeNormJ, timeMultiplyJJ, timeSplitJJ, 
		timeInvV, timeComposeZ, timeInvZ, timeTE, timeComposeC, timePtsUnc, timeAll;
    double lambda;
    int *fixedPts;
    std::vector<double> cycle_change;

    ~Statistic(){
        free(fixedPts);
    }
};

struct Uncertainty {
	std::size_t _camParams = 0;
	std::size_t _numCams = 0;
	std::size_t _numPoints = 0;
    std::size_t _nbCovarianceValuePerCam = 0;
    std::vector<double> _camerasUnc;
    std::vector<double> _pointsUnc;

	void init(const Options& options);

    /**
     * @return upper triangle of covariance matrix = 1/2*n(n+1) values per camera with n parametes
     */
	const std::vector<double> getCamerasUncRaw() const;
    
	/**
     * @return matrix of 6x6 values per point
     */
	const std::vector<double> getCameraUncMatrix(int id) const;
    
	/**
	* @return matrix of 6x6 values per point
	*/
	const std::vector<double> getCamerasUncMatrices() const;

	/**
     * @return 6 values per camera
     */
	const std::vector<double> getCamerasUncEigenValues() const;


    /**
     * @return 6 values per point
     */
	const std::vector<double> getPointsUncRaw() const;
    
	/**
     * @return matrix of 3x3 values per point
     */
	const std::vector<double> getPointUncMatrix(int id) const;
	
	/**
	* @return matrix of 3x3 values per point
	*/
	const std::vector<double> getPointsUncMatrices() const;

    /**
     * @return 3 values per point
     */
	const std::vector<double> getPointsUncEigenValues() const;
};
}

/**
 * @param[in] options: informations about the reconstruction (numCams, camParams, numPoints, numObs)
 * @param[out] statistic: blank object for the output statistics
 * @param[in] jacobian: sparse matrix (form Ceres-solver) with jacobian ( you can use the output of the computation of jacobian in Ceres as the input )
 * @param[in] points3D: all 3D points (to select the 3 static points)
 * @param[out] uncertainties: output covariances for cameras and points
 */
void getCovariances(
    cov::Options &options,
    cov::Statistic &statistic,
    ceres::CRSMatrix &jacobian,
    double* points3D,
    cov::Uncertainty& uncertainties);
