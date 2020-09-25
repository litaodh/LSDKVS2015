#pragma once
#include <string>
#include <memory>
#include "CalibrationAPI.h"

namespace tinyxml2 {
	enum XMLError;
	class XMLElement;
	class XMLDocument;
}

namespace ranger3calibration {

	class CalibrationParametersFactory
	{
	public:
		CalibrationParametersFactory();
		void loadFromFile(
			const std::string& pathToCalibrationXML,
			CalibrationParameters & outParameters);
		~CalibrationParametersFactory();
	private:
		void readOutHomography(tinyxml2::XMLElement* homography, double* H);
	};
}
