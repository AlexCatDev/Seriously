#pragma once

namespace MathUtils {

	void Clamp(float& value, float min, float max) {
		if (value > max)
			value = max;
		else if (value < min)
			value = min;
	}

	void ClampMin(float& value, float min) {
		if (value < min)
			value = min;
	}

	void ClampMax(float& value, float max) {
		if (value > max)
			value = max;
	}

	void Map(float& value, float fromSource, float toSource, float fromTarget, float toTarget) {
		value = (value - fromSource) / (toSource - fromSource) * (toTarget - fromTarget) + fromTarget;
	}

}