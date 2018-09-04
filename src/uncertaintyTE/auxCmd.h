// This file is part of the xxxxxxxxx project.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

/*
* File:   auxCmd.h
* Author: xxxxxxxxx xxxxxxxxx
*/

#pragma once	

#define _USE_MATH_DEFINES
#include <cmath>
#include "compute.h"


void replaceAll(std::string& str, const std::string& from, const std::string& to);

void loadJacobian(std::ifstream& file, int algorithm, ceres::CRSMatrix& jacobian, cov::Options& options);

void printJacobian(ceres::CRSMatrix &J);
void printJacobianMEX(ceres::CRSMatrix &J);
void setPts2Fix(cov::Options &opt, int N, double *pts);

#ifdef USE_OPENMVG
	int loadSceneOpenMVG(std::string sSfM_Data_Filename_In, openMVG::sfm::SfM_Data &sfm_data);
	void openmvgSfM2Jacobian(openMVG::sfm::SfM_Data &sfm_data, ceres::CRSMatrix &jacobian, cov::Options &opt);
#endif
       
