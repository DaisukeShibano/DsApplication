#ifndef _DS_INVERSE_MAT_ITERATION_
#define _DS_INVERSE_MAT_ITERATION_

namespace DsLib
{

	//dst�ɂ͏����l�����Ă����K�v����
	template<int SIZE>
	void DsIterationSolveSimultaneousEquation( const double srcVec[SIZE], const double srcMat[SIZE][SIZE], double dst[SIZE], const int iteNum )
	{
		// srcVec = srcMat * dst �ɂȂ�悤��dst�����߂�

		for( int trial=0; iteNum > trial; ++trial )
		{

			for (int col = 0; col < SIZE; ++col)
			{
				if( fabs(srcMat[col][col]) < 0.000001 ) continue;

				double forward = 0.0;
				for (int k = 0; k < col; ++k)
				{
					forward += (srcMat[col][k] * dst[k]);
				}
				double back = 0.0;
				for(int k=col+1; k < SIZE; ++k)
				{
					back += (srcMat[col][k] * dst[k]);
				}

				dst[col] = (srcVec[col] - forward - back)/srcMat[col][col];
			}
		}
	}

}



#endif