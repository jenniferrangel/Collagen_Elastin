#ifndef FUNCTOR_MISC_H_
#define FUNCTOR_MISC_H_

#include "system_structures.h"

__device__
inline CVec3 cross_product(CVec3 v1, CVec3 v2) {
	return thrust::make_tuple(thrust::get<1>(v1)*thrust::get<2>(v2) - thrust::get<2>(v1)*thrust::get<1>(v2),
		-(thrust::get<0>(v1)*thrust::get<2>(v2) - thrust::get<2>(v1)*thrust::get<0>(v2)),
		thrust::get<0>(v1)*thrust::get<1>(v2) - thrust::get<1>(v1)*thrust::get<0>(v2));
};

__device__
inline double dot_product(CVec3 v1, CVec3 v2) {
	return (thrust::get<0>(v1)*thrust::get<0>(v2) +
		thrust::get<1>(v1)*thrust::get<1>(v2) +
		thrust::get<2>(v1)*thrust::get<2>(v2));
}
//returns true if is greater than level
struct IsGreaterThanLevel {
	double limit;

	__host__ __device__ 
		IsGreaterThanLevel(
			double& _limit) : 
			limit(_limit) {}
		
	__device__
		bool operator() (double val) {

			return (val > limit);
		}
}; 

//returns true if is greater than level
struct IsGreaterOrLessThanLevel {
	double upperLimit;
	double lowerLimit;

	__host__ __device__ 
		IsGreaterOrLessThanLevel(
			double& _upperLimit,
			double& _lowerLimit) : 
			upperLimit(_upperLimit),
			lowerLimit(_lowerLimit) {}
		
	__device__
	//replaces value with 1 if returns true 
		bool operator() (double val) {
			if (val > upperLimit) {
				return true;
			}
			else if (val < lowerLimit) {
				return true;
			}
			else {
				return false;
			}
			
		}
}; 

//returns true if less than llevel.
struct IsLessThanLevel {
		double limit;
 
	__host__ __device__ 
		IsLessThanLevel(
			double& _limit) : 
			limit(_limit) {}
		
	__device__
	//replaces value with 1 if returns true 
	bool operator() (double val) {
		return (val <  abs( limit) );//((1-percentPull) * networkLength));
	}
};

struct functor_add_UCVec3_CVec3 {//same as torsion
	unsigned max_node_count;
	double* forceXAddr;
	double* forceYAddr;
	double* forceZAddr;

	__host__ __device__
	//
		functor_add_UCVec3_CVec3(
				unsigned& _max_node_count,
				double* _forceXAddr,
				double* _forceYAddr,
				double* _forceZAddr) :
			max_node_count(max_node_count),
			forceXAddr(_forceXAddr),
			forceYAddr(_forceYAddr),
			forceZAddr(_forceZAddr) {}

	__device__
	void operator() (const Tuddd& u1d3) {
			unsigned idToAssign = thrust::get<0>(u1d3);
			
			if (idToAssign < max_node_count) {
				if (!isnan(thrust::get<1>(u1d3)) && !isnan(thrust::get<2>(u1d3)) && !isnan(thrust::get<3>(u1d3))) {

					forceXAddr[idToAssign] += thrust::get<1>(u1d3);
					forceYAddr[idToAssign] += thrust::get<2>(u1d3);
					forceZAddr[idToAssign] += thrust::get<3>(u1d3);
				}
			}
	}

};


struct functor_add_UCVec3_CVec3_pltVol {
	unsigned max_node_count;
	double* forceXAddr;
	double* forceYAddr;
	double* forceZAddr;
	bool* isNodeInPltVolAddr;

	__host__ __device__
	//
		functor_add_UCVec3_CVec3_pltVol(
				unsigned& _max_node_count,
				double* _forceXAddr,
				double* _forceYAddr,
				double* _forceZAddr,
				bool* _isNodeInPltVolAddr) :
			max_node_count(max_node_count),
			forceXAddr(_forceXAddr),
			forceYAddr(_forceYAddr),
			forceZAddr(_forceZAddr),
			isNodeInPltVolAddr(_isNodeInPltVolAddr) {}

	__device__
	void operator() (const Tuddd& u1d3) {
			unsigned idToAssign = thrust::get<0>(u1d3);
			double f1 = thrust::get<1>(u1d3); 
			double f2 = thrust::get<2>(u1d3); 
			double f3 = thrust::get<3>(u1d3); 

			if (idToAssign < max_node_count) {
				if ((f1 != 0.0) || (f2 != 0.0) || (f3 !=0.0)) {

					forceXAddr[idToAssign] += thrust::get<1>(u1d3);
					forceYAddr[idToAssign] += thrust::get<2>(u1d3);
					forceZAddr[idToAssign] += thrust::get<3>(u1d3); 
					isNodeInPltVolAddr[idToAssign] = true;
				}
				if( (f1 == 0.0) && (f2 == 0.0) && (f3 == 0.0) ){
					isNodeInPltVolAddr[idToAssign] = false;
				}
			}


	}

};

struct functor_norm {

	__host__ __device__
		double operator() (const CVec3& vec) {
		//divide force by fiber cross section to get stress
		double result = sqrt(thrust::get<0>(vec) * thrust::get<0>(vec) +
			thrust::get<1>(vec) * thrust::get<1>(vec) +
			thrust::get<2>(vec) * thrust::get<2>(vec));

		return result;


	}
};


struct functor_prob_detach {//same as torsion
	double dt;
	double P;
	unsigned maxIdCountFlag;

	__host__ __device__
		//
		functor_prob_detach(
			double& _dt,
			double& _P,
			unsigned& _maxIdCountFlag):
		dt(_dt),
		P(_P),
		maxIdCountFlag(_maxIdCountFlag) {}

	__device__
		unsigned operator() (const Tud& u1d1) {
		unsigned current_node_plt_attached = thrust::get<0>(u1d1);
		double prob = thrust::get<1>(u1d1);

		if (prob < dt * P) {
			return maxIdCountFlag;
		}
		else {
			return current_node_plt_attached;
		}

	}

};

#endif