#include "ScalingFitFCN.h"
#include <cmath>

double ScalingFitFCN::operator()(const double* par){

	double chisq = 0;

	double	scaling	= par[0];

	for(int i=0;i<exptData.size();i++){
	  chisq	+= std::pow((scaling * calcData.at(i) - exptData.at(i))/exptUnc.at(i),2);
	}

	return	chisq;

}
