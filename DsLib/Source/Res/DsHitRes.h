#ifndef _DS_HITRES_H_
#define _DS_HITRES_H_

namespace DsLib
{
	class DsAnimRes;
	class DsResource;
}

namespace DsLib
{
	class DsHitRes
	{

	public:
		struct Shape
		{
			Shape()
				: sizeX(0)
				, sizeY(0)
				, sizeZ(0)
				, weight(0)
				, boundCoef(0)
			{}
			union
			{
				double params[5];
				struct
				{
					double sizeX;
					double sizeY;
					double sizeZ;
					double weight;
					double boundCoef;
				};
			};
		};

	public:
		DsHitRes();
		DsHitRes(const Shape& shape);
		~DsHitRes();

	public:
		void Initialize(const char* path, DsResource& resource);
		const DsAnimRes* GetAnimRes() const{ return m_pAnimRes; }
		const Shape& RefSpahe() const{ return m_shape; }

	private:
		DsAnimRes* m_pAnimRes;
		Shape m_shape;
	};

	
}

#endif