#pragma once

#include <string>

#include <Math/Common.h>
#include <Math/Vector.h>

namespace Refraction::Math {
	class Orientation {
	public:
		float mPitch = 0, mYaw = 0, mRoll = 0;

		// Create an identity Orientation
		Orientation() {}
		// Create an Orientation with the given angles
		Orientation(float pitch, float yaw, float roll) : mPitch(pitch), mYaw(yaw), mRoll(roll) {
			// I'm sorry in advance if you're reading this
			while (pitch < -360) pitch += 360;
			while (pitch > 360) pitch -= 360;
			while (yaw < -360) yaw += 360;
			while (yaw > 360) yaw -= 360;
			while (roll < -360) roll += 360;
			while (roll > 360) roll -= 360;
		}
		// Create an Orientation using Euler angles
		Orientation(const Vector3& angles) : Orientation(angles.x, angles.y, angles.z) {}

		inline Vector3 ToVector3() const { return Vector3(mPitch, mYaw, mRoll); }
		inline Vector3 ForwardVector() const {
			float sinP, cosP, sinY, cosY;
			SinCos(sinP, cosP, ToRadians(mPitch));
			SinCos(sinY, cosY, ToRadians(mYaw));
			return Vector3(cosP * cosY, cosP * sinY, sinP);
		}
		inline Vector3 RightVector() const {
			float sinP, cosP, sinY, cosY, sinR, cosR;
			SinCos(sinP, cosP, ToRadians(mPitch));
			SinCos(sinY, cosY, ToRadians(mYaw));
			SinCos(sinR, cosR, ToRadians(mRoll));
			return Vector3(sinR * sinP * cosY - cosR * sinY,
						   sinR * sinP * sinY + cosR * cosY,
						   cosP * sinR
			);
		}
		inline Vector3 UpVector() const {
			const float radP = ToRadians(mPitch);
			const float radY = ToRadians(mYaw);
			const float radR = ToRadians(mRoll);
			return Vector3(cosf(radR) * sinf(radP) * cosf(radY) + sinf(radR) * sinf(radY),
						   cosf(radR) * sinf(radP) * sinf(radY) - sinf(radR) * cosf(radY),
						   cosf(radP) * cosf(radR)
			);
		}

		inline bool operator==(const Orientation& other) const {
			return (mPitch == other.mPitch) && (mYaw == other.mYaw) && (mRoll == other.mRoll);
		}

		inline Orientation operator+(const Orientation& other) {
			return Orientation(mPitch + other.mPitch, mYaw + other.mYaw, mRoll + other.mRoll);
		}
		inline Orientation operator-(const Orientation& other) {
			return Orientation(mPitch - other.mPitch, mYaw - other.mYaw, mRoll - other.mRoll);
		}
		inline Orientation operator*(const Orientation& other) {
			return Orientation(mPitch * other.mPitch, mYaw * other.mYaw, mRoll * other.mRoll);
		}

		inline void operator+=(const Orientation& other) {
			mPitch += other.mPitch; mYaw += other.mYaw; mRoll += other.mRoll;
		}
		inline void operator-=(const Orientation& other) {
			mPitch -= other.mPitch; mYaw -= other.mYaw; mRoll -= other.mRoll;
		}
		inline void operator*=(const Orientation& other) {
			mPitch *= other.mPitch; mYaw *= other.mYaw; mRoll *= other.mRoll;
		}

		void Rotate(float pitch, float yaw, float roll);

		Orientation Rotate(float angle, const Vector3 &axis) const;

		inline std::string ToString(PrintFormatArgs fmtArgs = PrintFormatArgs()) const {
			if (fmtArgs.Pretty) {
				return std::string("Pitch: " + std::to_string(mPitch) + "\nYaw: " + std::to_string(mYaw) + "\nRoll: " + std::to_string(mRoll));
			} else {
				return std::string("{" + std::to_string(mPitch) + ", " + std::to_string(mYaw) + ", " + std::to_string(mRoll) + "}");
			}
		}
	private:
		inline void ClearNaNs() {
			if (mPitch != mPitch) mPitch = 0;
			if (mYaw != mYaw) mYaw = 0;
			if (mRoll != mRoll) mRoll = 0;
		}
	};
}
