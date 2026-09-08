#include <Math/Matrix.h>

#include "Orientation.h"

namespace Refraction::Math {
	void Orientation::Rotate(float pitch, float yaw, float roll) {
		mPitch += pitch;
		mYaw += yaw;
		mRoll += roll;

		while (mPitch < -360) mPitch += 360;
		while (mPitch > 360) mPitch -= 360;
		while (mYaw < -360) mYaw += 360;
		while (mYaw > 360) mYaw -= 360;
		while (mRoll < -360) mRoll += 360;
		while (mRoll > 360) mRoll -= 360;
	}

	Orientation Orientation::Rotate(float angle, const Vector3& axis) const {
		auto result = (Matrix4::FromRotation(*this) * Matrix4::FromRotation(angle, axis)).ToOrientation();
		result.ClearNaNs();
		return result;
	}
}
