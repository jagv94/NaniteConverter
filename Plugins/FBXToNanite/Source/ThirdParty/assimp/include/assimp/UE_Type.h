#pragma once
class IMPORTEXPORTLIBRARY_API UE_Type
{
public:
	class UEVector 
	{
	public:
		double x;
		double y;
		double z;

		UEVector() {}

		UEVector(double p_x, double p_y, double p_z)
			: x(p_x), y(p_y), z(p_z)
		{

		}

		UEVector operator-(const UEVector v) const
		{
			return UEVector(x - v.x, y - v.y, z - v.z);
		}

		UEVector operator*(const UEVector v) const
		{
			return UEVector(x * v.x, y * v.y, z * v.z);
		}
	};

	class UERotator
	{
	public:
		double pitch;
		double yaw;
		double roll;

		UERotator() {}

		UERotator(double p_pitch, double p_yaw, double p_roll)
			: pitch(p_pitch), yaw(p_yaw), roll(p_roll)
		{

		}
	};

public:
	class UETransform
	{
	public:
		UEVector location;
		UERotator roator;
		UEVector scale;

		UETransform() {}
	};
};

